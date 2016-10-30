#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "arvid.h"
#include "libarvid.h"



pthread_t thread;
extern arvid_private ap;

volatile char stopThread = 0;

static char frame_server_initialised = 0;

void* thread_runner(void* data) {
	unsigned short* dstFb[2];		//pru mini frame buffer in shared pru memory
	unsigned short* src;
	int width = ap.fbWidth;
	int height = ap.lines;
	int i;
	volatile int fbIndex = 0;
	int frame;
	volatile unsigned int frameNumber;
	struct sched_param schedParam;


	frame_server_initialised = 1;
	stopThread = 0;

	//printf("frame server w=%i h=%i\n", width, height);

	//each mini frame buffer is worth of 4 lines
	dstFb[0] = (unsigned short*)(&ap.pruSharedMem[4 + 340]);

	dstFb[1] = dstFb[0];
	dstFb[1] += width * 4;

	//set maximum taks priority to ensure smooth streaming
	i = sched_getparam(0, &schedParam);

	if (i) {
		printf("failed to get scheduling\n");
	} else {
		schedParam.sched_priority = sched_get_priority_max(SCHED_FIFO) - 2;
		i = sched_setscheduler(0, SCHED_FIFO, &schedParam);
		if (i) {
			printf("failed to set scheduling\n");
		}
	}

	while (1) {
		//wait for the start of the frame
		prussdrv_pru_wait_event(PRU_EVTOUT_1);
		prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU0_ARM_INTERRUPT);
//		prussdrv_pru_wait_event(PRU_EVTOUT_1);
//		prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU0_ARM_INTERRUPT);
		if (stopThread) {
			goto finish;
		}

		//set the source frame buffer
		frameNumber = ap.pruMem[PRU_DATA_FRAME_NUMBER];

		frame =  (frameNumber & 1);
		src = ap.fb[frame];
		i = height;
		fbIndex = 0;
		//copy one frame (one screen)
		do {
			memcpy(dstFb[fbIndex], src, width << 3); // * 8,  that is 4 lines * 2 bytes per pixel

			//if there is still some lines to render wait for the PRU signal
			if (i >= 4)
			{
				int line;
				prussdrv_pru_wait_event(PRU_EVTOUT_0);
				prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
//				prussdrv_pru_wait_event(PRU_EVTOUT_0);
//				prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
				if (stopThread) {
					goto finish;
				}

				//read current line
				line = ap.pruSharedMem[2];
				src = ap.fb[frame] + (width * line);
				fbIndex = (((line -1) & 7) >> 2);
				//printf("line=%03i fbLine=%03i fbIndex=%i \n", i, line, fbIndex);

			}
			i -= 4;						 // advance 4 lines

			//frame numbers are offsync -> early exit
			if (frameNumber != ap.pruMem[PRU_DATA_FRAME_NUMBER]) {
				i = 0;
			}
		} while (i > 0);

	}
finish:
	stopThread = 0;
//	printf("thread stopped!\n");
	return NULL;
}

int stop_frame_thread(void) {
//	printf("stopping the thread init=%i...\n", frame_server_initialised);
	if (frame_server_initialised) {
	  stopThread = 1;
	}
	return 0;
}

int start_frame_thread(void) {
	int result;
	char waitForInitialThread = 1;

	//a thread was already runing - wait till it's finished
	if (frame_server_initialised) {
//		printf("joining thread...\n");
		waitForInitialThread = 0;
		pthread_join(thread, NULL);
//		printf("thread joined...\n");
	}


	result = pthread_create(&thread, NULL, thread_runner, NULL);
	if (waitForInitialThread) {
	  usleep(5000);
	}
	return result;
}


