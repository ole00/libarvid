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
This example:
- initializes arvid (to default 320x240 PAL screen, 50 Hz)
- shows service video pattern
- waits 10 seconds
- switches videomode to 256x240 NTSC, 60Hz
- shows service video pattern
- waits 10 seconds
- gets and prints the button state
- closes arvid
*********************************************/

int main(int argc , char** argv) {
	int res;
	int lines;
	int buttons;

	//default video mode is PAL 320x240, 50 Hz,  304 total lines
	res = arvid_init();
	printf("example: arvid_init result=%i\n", res);

	//failed to initialize - bail out...
	if (res) {
		return 1;
	}

	printf("example: screen width=%i height=%i\n",
		arvid_get_width(), arvid_get_height());

	if (res == 0) {
		arvid_show_service_screen();
		usleep(10 *1000000);
	}

	//switch to NTSC 256x240, 60Hz, 262  total lines
	lines = arvid_get_video_mode_lines(arvid_256,60.0f); 
	res = arvid_set_video_mode(arvid_256, lines);
	printf("example: screen width=%i height=%i lines=%i\n",
		arvid_get_width(), arvid_get_height(), lines);

	if (res == 0) {
		arvid_show_service_screen();
		usleep(10 *1000000);
	}

	buttons = arvid_get_button_state();
	printf("buttons: coin=%i start=%i tate=%i\n",
	    buttons & ARVID_COIN_BUTTON ? 1 : 0,
	    buttons & ARVID_START_BUTTON ? 1 : 0,
	    buttons & ARVID_TATE_SWITCH ? 1 : 0
	);

	res = arvid_close();
	printf("example: arvid_close result=%i\n", res);

	return 0;
}