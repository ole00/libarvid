//
// Arvid software and hardware is licensed under MIT license:
//
// Copyright (c) 2015 Marek Olejnik
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this hardware, software, and associated documentation files (the "Product"),
// to deal in the Product without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Product, and to permit persons to whom the Product is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Product.
//
// THE PRODUCT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE PRODUCT OR THE USE OR OTHER DEALINGS
// IN THE PRODUCT.




#include "utils.p"

#define GPIO3 0x481ae000
#define GPIO_DATAIN 0x138
#define BLOCK_COUNT r3
#define STATE	r4

#define FRAME_BUFFER r5
#define PIXEL_BUFFER r6
#define PIXEL_ADDR   r7
#define DATA r8
#define DATA2 r9
#define DATA3 r10
#define FRAME_NUM  r26

#define STATE_ADDR 			0x10000
#define DDR_ADDR			0x10004
#define BLOCK_COUNT_ADDR 	 0x2008
#define FRAME_NUM_ADDR   	 0x2004
#define BUTTON_STATE_ADDR    0x2010
#define PIXEL_ADDR_START 	0x10010
#define FB_BLOCK_INDEX      0x10008

// To signal the host that we're done, we set bit 5 in our R31
// simultaneously with putting the number of the signal we want
// into R31 bits 0-3. See 5.2.2.2 in AM335x PRU-ICSS Reference Guide.
#define PRU0_R31_VEC_VALID (1<<5)
#define EVT_OUT_0 3 // corresponds to PRU_EVTOUT_0
#define EVT_OUT_1 4


// ****************************************
// Program start - PRU0 data streamer
// ****************************************
Start:
	//enable memory interconnect
	LBCO r0, C4, 4, 4
	CLR  r0, r0, 4
	SBCO r0, C4, 4, 4

	//synchronization  with PRU1
	// wait until STATE is equal 0xAC
	mov r0, STATE_ADDR
init_sync:
	lbbo STATE, r0, 0, 4
	qbne init_sync, STATE, 0xAC


//set the block count
	mov r0, BLOCK_COUNT_ADDR
	lbbo BLOCK_COUNT, r0, 0, 4

// note: top 16 bits of BLOCK_COUNT contain
// frame buffer address shift!

//set up frame index
	mov FRAME_NUM, 0

// =============================================================
// prepare pixels in the buffer 
// =============================================================
// 320 pixels

//set up inital PIXEL_ADDR of the generated buffer
	mov PIXEL_ADDR, PIXEL_ADDR_START
	add PIXEL_ADDR, PIXEL_ADDR, 200	//reserve 800 bytes (400 pixels) for the PRU1 line buffer
	add PIXEL_ADDR, PIXEL_ADDR, 200
	add PIXEL_ADDR, PIXEL_ADDR, 200
	add PIXEL_ADDR, PIXEL_ADDR, 200


// =============================================================
// Single frame sreaming start
// =============================================================
streaming_start:

	mov FRAME_BUFFER, PIXEL_ADDR	//source 


	//store the FRAME_NUM so the host can read it
	mov r0, FRAME_NUM_ADDR 
	sbbo FRAME_NUM, r0, 0, 4

	//* signal the interrupt to the streamer
	mov r31.b0, PRU0_R31_VEC_VALID | EVT_OUT_1


	//setup next frame number (back buffer host can safely write to)
	add FRAME_NUM, FRAME_NUM, 1

	mov PIXEL_BUFFER, PIXEL_ADDR_START		//destination address


// stream 320 pixel per line 
//we will read 64 bytes at once (16 registers, 32 pixels)
// note: top 16 bits of BLOCK_COUNT contain
// frame buffer address shift!
// use bottom 16 bits only

	//copy 8 lines
	mov r2, 8

	//save current block index
	mov r0, FB_BLOCK_INDEX
	sbbo r2, r0, 0 , 4

	//* signal the interrupt to the streamer
	mov r31.b0, PRU0_R31_VEC_VALID | EVT_OUT_0

	mov r0, BLOCK_COUNT.w0 		// count 10 * 32 = 640 bytes =>320 pixels (2 bytes per pixel)


//Single line streaming start
copy_line:
	lbbo DATA, FRAME_BUFFER, 0, 64  		//read from src  cca 46 cycles

	sbbo DATA, PIXEL_BUFFER, 0, 64			//write to shared memory 6 cycles
	add FRAME_BUFFER, FRAME_BUFFER, 64		//increase memory pointers
	add PIXEL_BUFFER, PIXEL_BUFFER, 64
	sub r0, r0, 1						//decrease pixel block count

	qbne copy_line, r0, 0				//copy until the end of line

	//subtract extra bytes from frame buffer address if the frame buffer
	//width is not multiply of 32 pixels (64 bytes). Use top 16 bits of the BLOCK_COUNT.
	sub FRAME_BUFFER, FRAME_BUFFER, BLOCK_COUNT.w2

	//reset destination address
	mov PIXEL_BUFFER, PIXEL_ADDR_START

	//synchronize with PRU1
	//wait till the line has been drawn
	//basically if STATE is not 0 it means new line can be rendered
    // state value 1 means new line, state value 2 means start
  	// from the beginning of the frame
	mov r0, STATE_ADDR
line_sync:
	lbbo STATE, r0, 0, 4
	qbeq line_sync, STATE, 0x0

	//clear sync
	mov r1, 0
	sbbo r1, r0, 0, 4

	//decrese mini frame buffer line index
	sub r2, r2, 1

	//if not the 4th line (index 4) then continue to other check
	qbne line_check, r2, 4
	//* this is the 4th line -> signal the interrupt to the streamer

	//save current block index (r2)
	mov r0, FB_BLOCK_INDEX
	sbbo r2, r0, 0 , 4

	mov r31.b0, PRU0_R31_VEC_VALID | EVT_OUT_0

line_check:
	qbne line_finish, r2, 0	//if not the 8th line (index 0) then continue to line_finish
	//otherwise (when counter r2 == 0):


	//* reset the mini frame buffer
	mov FRAME_BUFFER, PIXEL_ADDR	//source 
	mov r2, 8

	//save current block index (r2)
	mov r0, FB_BLOCK_INDEX
	sbbo r2, r0, 0 , 4

	//* signal the interrupt to the streamer
	mov r31.b0, PRU0_R31_VEC_VALID | EVT_OUT_0

line_finish:
	qbeq streaming_start, STATE, 2		//state is 2 - start over new frame

	mov r0, BLOCK_COUNT.w0
	qba copy_line					//copy next line

