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
- enumerates video modes
- cycles through video modes and shows the service screen
- exits when coin button is pressed
*********************************************/

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
			//check whether the coin button was pressed then exit
		    if (buttons & ARVID_COIN_BUTTON) {
			  return -1;
			}
		}
		return 0;
	} else {
	    printf("example: error - failed to set videomode\n");
	    return -1;
	}
}

int main(int argc , char** argv) {
	int res;
	arvid_vmode_info vmodes[32];
	int maxVmode = 32;

	//default video mode is PAL 320x240, 50 Hz,  304 total lines
	res = arvid_init();
	printf("example: arvid_init result=%i\n", res);

	//failed to initialize - bail out...
	if (res) {
		return 1;
	}

	res = arvid_enum_video_modes(vmodes, &maxVmode);
	//failed to enumerate - bail out...
	if (res) {
	  return 1;
	}

	printf("Press & hold [Coin] button to exit!\n");

	while (1)
	{
		int i;
		//display all videomodes
		for (i = maxVmode - 1; i >= 0; i--) {
		  res = set_vmode(vmodes[i].vmode, "");
		  if (res < 0) {
			goto finish;
		  }
		}
	}

finish:
	res = arvid_close();
	printf("example: arvid_close result=%i\n", res);

	return 0;
}