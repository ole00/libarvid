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
#include <unistd.h>

#include "arvid.h"

/********************************************
This example sets videomode with different number of lines
and calculate framerates
*********************************************/

arvid_video_mode videoMode = arvid_292;

static void test(int lines, int seconds) {
//	printf("test lines=%i seconds=%i mode=%i\n",lines, seconds, videoMode);
	int res = arvid_set_video_mode(videoMode, lines);
	printf("{%i, ", lines);
	if (res == 0) {
		unsigned frame;
		usleep(seconds *1000000);
		frame = arvid_get_frame_number();
		printf(" %f},\n", (frame / (seconds * 1.0f)));
	} else {
	    printf("failed to set videomode\n");
	}
}

int main(int argc , char** argv) {
	int res;
	int lines;
	int maxLines;

	if (argc > 1) {
		videoMode = (arvid_video_mode) atoi(argv[1]);
	}

	printf("using video mode=%i\n", videoMode);

	//default video mode is PAL 320x240, 50 Hz,  304 total lines
	res = arvid_init();

	//failed to initialize - bail out...
	if (res) {
		return 1;
	}

	//lines = arvid_get_video_mode_lines(5000); // 50.00 Hz
	//maxLines = arvid_get_video_mode_lines(6000); //60.00 Hz
	lines = 304;
	maxLines = 248;

	printf("please wait (test takes ~45 minutes) ....\n");
	arvid_show_service_screen();
	printf("quick test 5 sec. Expected 60.00 Hz\n");
	test(252, 5);
	//printf("normal test 1 min. Expected 60.00 Hz\n");
	//test(252, 60);
	printf("long test 5 min. Expected 60.00 Hz\n");
	test(252, 5 * 60);
	//test(304, 60);
	//test(279, 60);

	while (lines >= maxLines) {
		test(lines,60);
		lines--;
	}

	res = arvid_close();

	return 0;
}