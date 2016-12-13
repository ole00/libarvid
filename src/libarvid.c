/*
Arvid software and hardware is licensed under MIT license:

Copyright (c) 2015 Marek Olejnik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this hardware, software, and associated documentation files (the "Product"),
to deal in the Product without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Product, and to permit persons to whom the Product is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Product.

THE PRODUCT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE PRODUCT OR THE USE OR OTHER DEALINGS
IN THE PRODUCT.

*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

//#include "libdmacopy.h"

#include <arvid.h>
#include "libarvid.h"
#include "blitter.h"
#include "frame_server.h"

#define ARVID_BUTTON_MASK (ARVID_START_BUTTON | ARVID_COIN_BUTTON | ARVID_TATE_SWITCH)

//PRU Data:
//byte 0  - 63 : header: video mode registers and misc info (16 integers)
//byte 64 - XX : frame buffer data - individal pixels of a single line
//               in ARGB1555 format streamed from DDR


//corresponds to videomodes (in chronological order)
static int arvid_resolution[] = {
	320,
	256,
	288,
	384,
	240,
	392,
	400,
	292,
	336,
};

//vmode table - must be sorted in descending oorder
static arvid_vmode_info vmode_info[] = {
    { 400, arvid_400 },
    { 392, arvid_392 },
    { 384, arvid_384 },
    { 336, arvid_336 },
    { 320, arvid_320 },
    { 292, arvid_292 },
    { 288, arvid_288 },
    { 256, arvid_256 },
    { 240, arvid_240 },
};

arvid_private ap = {
		0,
		0,
		0,
		0,
		0,
		-1,
		60  // linePosMod -> affects X position
};


extern arvid_line_rate arvid_rate_table[arvid_last_video_mode][RATE_SIZE];


static int get_uio_address_(int id) {
	char device[64];
	char buffer[32];
	int fd;
	int size;

	sprintf(device, "/sys/class/uio/uio%i/maps/map1/addr", id);

	fd = open(device, O_RDONLY);
	if (fd < 0) {
		return -1;
	}

	memset(buffer, 0, 32);
	size = read(fd, buffer, 32);
	close(fd);
	if (size >  5) {
		sscanf(buffer, "%x", &ap.ddrAddress);
		printf("arvid: found ddr address at: 0x%08x\n", ap.ddrAddress);
	}

	return 0;
}

/* initialize pruss */
static int init_pruss_(void) {
	int res;
	int i;
	tpruss_intc_initdata intc = PRUSS_INTC_INITDATA;

	//run modprobe
	res = system("/sbin/modprobe uio_pruss  extram_pool_sz=0x400000");
	if (res < 0) {
		printf("arvid: modprobe uio_pruss failed: %i\n", res);
		return ARVID_ERROR_PRU_INIT_FAILED;
	}
	usleep(100000);

	res = system("echo pru_arvid > /sys/devices/bone_capemgr.9/slots");
	if (res < 0) {
		printf("arvid: dtbo overlay failed: %i\n", res);
		return ARVID_ERROR_PRU_INIT_FAILED;
	}
	usleep(100000);

	//try to find uio address
	ap.ddrAddress = 0;
	for (i = 0; i < 8; i++) {
		res = get_uio_address_(i);
		if (res == 0 && ap.ddrAddress != 0) {
			break;
		}
	}
	if (ap.ddrAddress == 0) {
		printf("arvid: uio address lookup failed.\n");
		return ARVID_ERROR_PRU_INIT_FAILED;
	}

	res = prussdrv_init();
	if (res) {
		printf("arvid: prussdrv_init failed: %i\n", res);
		return ARVID_ERROR_PRU_INIT_FAILED;
	}

	//setup interrupt events

	res = prussdrv_open(PRU_EVTOUT_0);
	if (res) {
		printf("arvid: prussdrv_open 0 failed: %i\n", res);
		return ARVID_ERROR_PRU_INIT_FAILED;
	}

	res = prussdrv_open(PRU_EVTOUT_1);
	if (res) {
		printf("arvid: prussdrv_open 1 failed: %i\n", res);
		return ARVID_ERROR_PRU_INIT_FAILED;
	}

	//test

	/* initialize interrupt */
	res = prussdrv_pruintc_init(&intc);
	if (res) {
		printf("arvid: prussdrv_pruintc_init() failed: %i\n", res);
		return res;
	}
	//test end

	return 0;
}

/* setup pru data related to video mode and rendering */
static void setPruMem(int fbWidth, int fbLines) {
	int blockCnt;

	ap.pruMem[PRU_DATA_FB_ADDR] = (unsigned int) ap.ddrAddress;
	//note: pruMem[1] contains current frame number

	//set BLOCK_COUNT to PRU (horizontal resolution)
	blockCnt = fbWidth / 32;
	if ((fbWidth % 32) == 0) {
		ap.pruMem[PRU_DATA_BLOCK_COUNT] = blockCnt; //stream 32 pixels per block (64 bytes) 
	} else {
		//stream one more block
		blockCnt ++;
		//set flag to propelry adjust the next line address
		//note: '<< 17' means 16 + 1. 16 to offset to high word, 1 to multiply the value * 2 
		//to get number of bytes to detract from the frame-buffer address
		ap.pruMem[PRU_DATA_BLOCK_COUNT] = (blockCnt) | ((32 - (fbWidth % 32)) << 17) ; //stream 32 pixels per block (64 bytes) 
	}

	//set TOTAL_LINES to PRU (vertical resolution)
	ap.pruMem[PRU_DATA_TOTAL_LINES] = fbLines;

	ap.pruMem[PRU_DATA_LINE_POS_MOD] = ap.linePosMod;
}

/* initialize memory mapping */
int init_memory_mapping_(void) {
	int fd;
	//int ret;
	void * mem;

	//map internal pru memory 
	prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &mem);
	ap.pruMem = (unsigned int*) mem;

	prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, &mem);
	ap.pruSharedMem = (unsigned int*) mem;
	printf("shared mem=%p\n", ap.pruSharedMem);

	//set video mode info to pru
	setPruMem(INITIAL_FB_W, INITIAL_FB_LINES);

	//map DDR memory

	fd = open("/dev/mem", O_RDWR);
	if (fd < 0) {
		printf("arvid: failed to open /dev/mem %i\n", fd);
		return ARVID_ERROR_PRU_MEMORY_MAPPING_FAILED;
	}
	ap.ddrFd = fd;
	ap.ddrMem = (unsigned int*) mmap(0, 0x400000, PROT_READ | PROT_WRITE, MAP_SHARED, ap.ddrFd, ap.ddrAddress);


/*
	ap.ddrFd = -1;
	ap.ddrMem = (unsigned int*) dma_malloc(0x200000); // allocate 2 MBytes of dma memory
	if (ap.ddrMem == NULL) {
		printf("arvid: failed allocate DMA memory\n");
		return ARVID_ERROR_PRU_MEMORY_MAPPING_FAILED;
	}
*/

//	ap.ddrFd = -1;
//	ap.ddrMem = (unsigned int*) malloc(0x200000); // allocate 2 MBytes of system memory
	if (ap.ddrMem == NULL) {
		printf("arvid: failed allocate DMA memory\n");
		return ARVID_ERROR_PRU_MEMORY_MAPPING_FAILED;
	}

	ap.ddrMem[0] = 0;

	//add shared pru memory to dma mapping (12 KBytes)
/*
	ret = dma_add_buffer(mem, 0x4a310000, 12 * 1024);
	if (ret) {
		printf("arvid: failed to map PRU shared memory to dma\n");
		return ARVID_ERROR_PRU_MEMORY_MAPPING_FAILED;
	}
*/
	return 0;
}

static int load_pru_code_(arvid_video_mode mode) {
	int res;
	char fileName[64];

	res = prussdrv_exec_program(0, "./prustream.bin");
	if (res != 0) {
		printf("arvid: failed to load prustream.bin\n");
		return ARVID_ERROR_PRU_LOAD_FAILED;
	}

	sprintf(fileName, "./pruvideo_%i.bin", arvid_resolution[mode]);
	res = prussdrv_exec_program(1, fileName);
	if (res != 0) {
		printf("arvid: failed to load %s\n", fileName);
		return ARVID_ERROR_PRU_LOAD_FAILED;
	}
	return 0;
}

static void init_frame_buffer_(arvid_video_mode mode, int lines, int noFbClear) {
	ap.lines = lines;
	ap.fbWidth = arvid_resolution[mode];
	ap.fbHeight = INITIAL_FB_H;
	//printf("mode set=%i w=%i lines=%i\n", mode, ap.fbWidth, ap.lines);
	ap.fb[0] = (unsigned short*) &ap.ddrMem[16];
	ap.fb[1] = (unsigned short*) &ap.ddrMem[16 + (0x100000 >> 2)]; //4 bytes per int

	if (noFbClear == 0) {
		//clean both frames with black color
		arvid_fill_rect(0, 0, 0, ap.fbWidth, ap.lines, 0);
		arvid_fill_rect(1, 0, 0, ap.fbWidth, ap.lines, 0);
	}
}

/* loads pru code, initializes memory  mappings etc */
int arvid_init_ex(int initFlags) {
	int res;

	//check already initialized
	if (ap.initialized == 0xACCE5503) {
		return 0;
	}

	if (geteuid()) {
		/* has to be root ! */
		printf("arvid: error ! permission check failed. Superuser required.\n");
		return ARVID_ERROR_PRU_PERMISSION_REQUIRED;
	}

/*
	res = dma_init();
	if (res) {
		printf("arvid: failed to initiialise dmacopy: %i\n", res);
		return ARVID_ERROR_DMA_INIT_FAILED;
	}

*/
	res = init_pruss_();
	if (res) {
		return res;
	}

	res = init_memory_mapping_();
	if (res) {
		return res;
	}

	init_frame_buffer_(INITIAL_VIDEO_MODE, INITIAL_FB_LINES, initFlags & FLAG_NO_FB_CLEAR);

	res = load_pru_code_(INITIAL_VIDEO_MODE);
	if (res) {
		return res;
	}

	res = start_frame_thread();
	if (res) {
		printf("arvid: failed to start frame thread: %i\n", res);
		return ARVID_ERROR_THREAD_FAILED;
	}

	ap.initialized = 0xACCE5503;
	return 0;
}

/* backward compatible init function */
int arvid_init() {
	return arvid_init_ex(0);
}


static int arvid_disable_pruss_(void) {
	int error = 0;
	int res;

	res = prussdrv_pru_disable(0);
	if (res) {
		printf("arvid: prussdrv_pru_disable(0) failed: %i\n", res);
		error++;
	}

	res = prussdrv_pru_disable(1);
	if (res) {
		printf("arvid: prussdrv_pru_disable(1) failed: %i\n", res);
		error++;
	}
	return error;
}

int arvid_close(void) {
	int res;
	int error = 0;

	//check not yet initialized
	ARVID_INIT_CHECK;
	error = arvid_disable_pruss_();

	res = prussdrv_exit();
	if (res) {
		printf("arvid: prussdrv_exit failed: %i\n", res);
		error++;
	}


	if (ap.ddrMem != NULL) {
		munmap(ap.ddrMem, 0x400000);
		ap.ddrMem = NULL;
	}

/*
	if (ap.ddrMem != NULL) {
		free(ap.ddrMem);
		ap.ddrMem = NULL;
	}
*/
	if (ap.ddrFd >= 0) {
		close(ap.ddrFd);
		ap.ddrFd = -1;
	}

/*
	res = dma_exit();
	if (res) {
		printf("arvid: dma_exit() failed: %i\n", res);
		error++;
	}
*/
	if (error == 0) {
		ap.initialized = 0;
		return 0;
	}
	return ARVID_ERROR_PRU_CLOSE_FAILED ;
}

unsigned short* arvid_get_frame_buffer(int index) {

	//check not yet initialized
	if (ap.initialized != 0xACCE5503) {
		return NULL;
	}

	if (index < 0 || index > 1) {
		return NULL;
	} 

	return ap.fb[index];
}

unsigned int arvid_get_frame_buffer_size(void) {
	//check not yet initialized
	ARVID_INIT_CHECK;

	return ap.lines * ap.fbWidth;
}

int arvid_wait_for_vsync(void) {
	//check not yet initialized
	ARVID_INIT_CHECK;
	volatile unsigned int frame = ap.pruMem[PRU_DATA_FRAME_NUMBER];
	unsigned int arvidStartFrame_ = frame;

	//check frame number has changed
	while (frame == arvidStartFrame_) {
		usleep(37);
		frame = ap.pruMem[PRU_DATA_FRAME_NUMBER];
	}
//	arvidStartFrame_ = frame;
	return 1 - (frame & 1);
}

int arvid_get_button_state(void) {
	//check not yet initialized
	ARVID_INIT_CHECK;
	{
		//gpi buttons are pulled high by default, we have to invert them
		volatile unsigned int buttons = ~ap.pruMem[PRU_DATA_GPIO_STATE];
		return (unsigned int) (buttons & ARVID_BUTTON_MASK);
	}
}

int arvid_get_width(void) {
	//check not yet initialized
	ARVID_INIT_CHECK;
	return ap.fbWidth;
}

int arvid_get_height(void) {
	//check not yet initialized
	ARVID_INIT_CHECK;
	return ap.lines;
}

int arvid_get_video_mode_lines(arvid_video_mode mode, float frate) {
	int i;
	float minDiff = 1000.0f;
	float diff;
	int minIndex = 0;
	arvid_line_rate* table;

	//limit rates to sane values
	if (frate < 50.0f) {
		frate = 50.0f;
	}

	if (frate > 61.0f) {
		frate = 61.0f;
	}

	//check video mode
	if (mode < 0 || mode >= arvid_last_video_mode) {
		printf("arvid: unknown video mode %i\n", (int) mode);
		return ARVID_ERROR_ILLEGAL_VIDEO_MODE;
	}

	//search through frame rate table
	table = arvid_rate_table[(int) mode];
	for (i = 0; i < RATE_SIZE; i++) {
		arvid_line_rate item = table[i];
		diff = frate - item.rate;
		if (diff < 0) {
			diff = -diff;
		}
		if (diff < minDiff && item.rate <= frate) {
			minDiff = diff;
			minIndex = i;
		}
	}

	//printf("arvid: rate-lines minIndex = %i, minDiff=%f frameRate=%f\n", minIndex, minDiff, table[minIndex].rate);
	return table[minIndex].line ;
}

float arvid_get_video_mode_refresh_rate(arvid_video_mode mode, int lines) {
	int i;
	arvid_line_rate* table;

	//check video mode
	if (mode < 0 || mode >= arvid_last_video_mode) {
		printf("arvid: unknown video mode %i\n", (int) mode);
		return 0;
	}

	//search through frame rate table
	table = arvid_rate_table[(int) mode];
	for (i = 0; i < RATE_SIZE; i++) {
		arvid_line_rate item = table[i];
		if (item.line == lines) {
			return item.rate;
		}
	}
	return 0;
}

int arvid_set_video_mode(arvid_video_mode mode, int lines) {
	ARVID_INIT_CHECK;

	if (lines < 248 || lines > 304) {
		printf("arvid: incorrect video mode lines %i\n", lines);
		return ARVID_ERROR_ILLEGAL_VIDEO_MODE;
	}

	if (mode < 0 || mode >= arvid_last_video_mode) {
		printf("arvid: unknown video mode %i\n", (int) mode);
		return ARVID_ERROR_ILLEGAL_VIDEO_MODE;
	}

	stop_frame_thread();
	usleep(10 * 1000);

	//set video mode info to pru
	setPruMem(arvid_resolution[mode], lines);

	//reset prus initial synchronisation state
	ap.ddrMem[0] = 0;

	//clean frame buffer
	init_frame_buffer_(mode, lines, 0);

	//arvidStartFrame_ = 0;

	//return 
	load_pru_code_(mode);
	return start_frame_thread();
}

int arvid_enum_video_modes(arvid_vmode_info* vmodeInfo, int* maxItems) {
	if (NULL == vmodeInfo) {
		if (NULL == maxItems) {
			return -1;
		}
		*maxItems = arvid_last_video_mode;
		return -1;
	}

	if (NULL == maxItems) {
		return -1;
	}

	//passed buffer is not big enough
	if (*maxItems < arvid_last_video_mode) {
		*maxItems = arvid_last_video_mode;
		return -1;
	}

	*maxItems = arvid_last_video_mode;
	memcpy(vmodeInfo, vmode_info, sizeof(arvid_vmode_info) * arvid_last_video_mode);
	return 0;
}


unsigned int arvid_get_frame_number(void) {
	//check not yet initialized
	if (ap.initialized != 0xACCE5503) {
		return 0;
	}

	return (unsigned int) ap.pruMem[PRU_DATA_FRAME_NUMBER];
}

void arvid_set_line_pos(int mod) {
	//check not yet initialized
	if (ap.initialized != 0xACCE5503) {
		return;
	}
	//printf("line pos set: %i\n", mod);
	if (mod < 1) {
		mod = 1;
	} else
	if (mod > 111) {
		mod = 111;
	}
	ap.linePosMod = mod;
	ap.pruMem[PRU_DATA_LINE_POS_MOD] = mod;
}

int arvid_get_line_pos() {
	//check not yet initialized
	if (ap.initialized != 0xACCE5503) {
		return 0;
	}
	return ap.linePosMod;
}
