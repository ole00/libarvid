/*
Arvid software and hardware is licensed under MIT license:

Copyright (c) 2015 - 2016 Marek Olejnik

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
- cycles through video modes
- exits when coin button is pressed
*********************************************/

#define CHECK_EXIT if (res < 0) break

static int set_vmode(int mode, char* message)
{
	int lines;
	int buttons;
	int res;
	int i;

	lines = arvid_get_video_mode_lines(mode, 60.0f);
	res = arvid_set_video_mode(mode, lines);

	if (res == 0) {
		arvid_show_service_screen();
		printf("example: screen width=%i height=%i lines=%i result=%i\n",
			arvid_get_width(), arvid_get_height(), lines, res);
		for (i = 0; i < 10; i++) {
		    usleep(1000000); //wait 1 sec
		    buttons = arvid_get_button_state();
		    if (buttons & ARVID_COIN_BUTTON) return -1;
		}
		return 0;
	} else {
	    printf("example: error - failed to set videomode\n");
	    return -1;
	}
}

int main(int argc , char** argv) {
	int res;

	//default video mode is PAL 320x240, 50 Hz,  304 total lines
	res = arvid_init();
	printf("example: arvid_init result=%i\n", res);

	//failed to initialize - bail out...
	if (res) {
		return 1;
	}
	//must be called before setting a videomode!
	arvid_show_service_screen();

	while (1)
	{
	    res = set_vmode(arvid_240, "vmode 240");
	    CHECK_EXIT;
	
	    res = set_vmode(arvid_256, "vmode 256");
	    CHECK_EXIT;

	    res = set_vmode(arvid_288, "vmode 288");
	    CHECK_EXIT;

	    res = set_vmode(arvid_292, "vmode 292");
	    CHECK_EXIT;

	    res = set_vmode(arvid_320, "vmode 320");
	    CHECK_EXIT;

	    res = set_vmode(arvid_384, "vmode 384");
	    CHECK_EXIT;

	    res = set_vmode(arvid_392, "vmode 392");
	    CHECK_EXIT;

	    res = set_vmode(arvid_400, "vmode 400");
	    CHECK_EXIT;
	}

	res = arvid_close();
	printf("example: arvid_close result=%i\n", res);

	return 0;
}