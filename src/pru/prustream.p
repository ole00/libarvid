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


#define FRAME_BUFFER r5
#define PIXEL_BUFFER r6
#define PIXEL_ADDR   r7
#define DATA r8
#define FRAME_BUFFER_2 r25
#define FRAME_NUM  r26
#define BLOCK_COUNT r3
// r2 is unused

#define STATE	r4
#define STATE_ADDR 			0x10000
#define DDR_ADDR			0x10004
#define BLOCK_COUNT_ADDR 	 0x2008
#define FRAME_NUM_ADDR   	 0x2004
#define PIXEL_ADDR_START 	0x10010


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


// read DDR address into FRAME_BUFFER from the data ram
// the addres was set up by the PRU1
	mov r0, DDR_ADDR
	lbbo FRAME_BUFFER, r0, 0, 4

// set the framebuffer 2 address -> one meg after the first FB
	mov r0, 0x100000
	add FRAME_BUFFER_2, FRAME_BUFFER, r0

//set the block count
	mov r0, BLOCK_COUNT_ADDR
	lbbo BLOCK_COUNT, r0, 0, 4

// note: top 16 bits of BLOCK_COUNT contain
// frame buffer address shift!

//set up frame index
	mov FRAME_NUM, 0

// =============================================================
// Streaming start
// =============================================================
streaming_start:

	//flip buffers according bit 0 in FRAME_NUM
	qbbs odd_index_buffer, FRAME_NUM, 0

	//set the even buffer
	mov PIXEL_BUFFER, FRAME_BUFFER
	qba streaming // jump over to streaming

odd_index_buffer:
	//set the odd buffer
	mov PIXEL_BUFFER, FRAME_BUFFER_2


streaming:
	//store the FRAME_NUM so the host can read it
	mov r0, FRAME_NUM_ADDR 
	sbbo FRAME_NUM, r0, 0, 4

	//setup next frame number (back buffer host can safely write to)
	add FRAME_NUM, FRAME_NUM, 1


	//stream 320 pixel per line 
	//we will read 64 bytes at once (16 registers, 32 pixels)

	mov PIXEL_ADDR, PIXEL_ADDR_START


// note: top 16 bits of BLOCK_COUNT contain
// frame buffer address shift!

// use bottom 16 bits only
	mov r0, BLOCK_COUNT.w0 		// count 10 * 32 = 640 bytes =>320 pixels (2 bytes per pixel)


copy_line:
	lbbo DATA, PIXEL_BUFFER, 0, 64  		//read from DDR  cca 46 cycles

	sbbo DATA, PIXEL_ADDR, 0, 64			//write to shared memory 6 cycles
	add PIXEL_BUFFER, PIXEL_BUFFER, 64		//increase memory pointers
	add PIXEL_ADDR, PIXEL_ADDR, 64
	sub r0, r0, 1						//decrease pixel block count

	qbne copy_line, r0, 0				//copy until the end of line

	//subtract extra bytes from pixel buffer address if the frame buffer
	//width is not multiply of 32 pixels (64 bytes). Use top 16 bits of the BLOCK_COUNT.
	sub PIXEL_BUFFER, PIXEL_BUFFER, BLOCK_COUNT.w2

	//reset destination address
	mov PIXEL_ADDR, PIXEL_ADDR_START

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

	qbeq streaming_start, STATE, 2		//state is 2 - start over new frame

	mov r0, BLOCK_COUNT.w0
	qba copy_line					//copy next line

