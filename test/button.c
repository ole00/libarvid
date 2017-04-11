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
- reads and prints buttons
- closes arvid
*********************************************/

int main(int argc , char** argv) {
	int res;
//	int lines;
	int b;

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
	}
	printf("   ---------------------------------------------\n");
	printf("     pp pp p | pppp   Tp | pp T Sppp | Tppp   pp\n");
	printf("     11 11 1 | 2222   i2 | 21 a e122 | e122   11\n");
	printf("        BB   |        l  |    t r BB | s BB   BB\n");
	printf("     RL 21 U | UDLR   tS | CC e vS14 | tD32   43\n");
	printf("   ---------------------------------------------\n");
	printf("   0000 0000 | 0011 1111 | 1111 2222 | 2222 2233\n");
	printf("   0123 4567 | 8901 2345 | 6789 0123 | 4567 8901\n");
	printf("   ----------|-----------|-----------|----------\n");

    while (1) {
		arvid_wait_for_vsync();
	   b = arvid_get_button_state();
#if 0
	   printf("buttons 0x%08x: coin=%i start=%i tate=%i\r",
		buttons,
	    buttons & ARVID_COIN_BUTTON ? 1 : 0,
	    buttons & ARVID_START_BUTTON ? 1 : 0,
	    buttons & ARVID_TATE_SWITCH ? 1 : 0
	   );
#endif
		printf("   %i%i%i%i %i%i%i%i | %i%i%i%i %i%i%i%i | %i%i%i%i %i%i%i%i | %i%i%i%i %i%i%i%i\r",
			b & 1, (b & (1<< 1))>>1, (b & (1<< 2))>>2, (b & (1<<3))>>3,
			(b & (1 << 4))>>4, (b & (1<< 5))>>5, (b & (1<< 6))>>6, (b & (1<<7))>>7,
			(b & (1 << 8))>>8, (b & (1<< 9))>>9, (b & (1<< 10))>>10, (b & (1<<11))>>11,
			(b & (1 << 12))>>12, (b & (1<< 13))>>13, (b & (1<< 14))>>14, (b & (1<<15))>>15,
			(b & (1 << 16))>>16, (b & (1<< 17))>>17, (b & (1<< 18))>>18, (b & (1<<19))>>19,
			(b & (1 << 20))>>20, (b & (1<< 21))>>21, (b & (1<< 22))>>22, (b & (1<<23))>>23,
			(b & (1 << 24))>>24, (b & (1<< 25))>>25, (b & (1<< 26))>>26, (b & (1<<27))>>27,
			(b & (1 << 28))>>28, (b & (1<< 29))>>29, (b & (1<< 30))>>30, (b & (1<<31)) ? 1 : 0
		);
		fflush(0);
		//usleep(1* 1000000);
	}

	res = arvid_close();
	printf("example: arvid_close result=%i\n", res);

	return 0;
}