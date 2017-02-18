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
#include "arvid.h"
#include "blitter.h"
#include "libarvid.h"

extern arvid_private ap;

void arvid_fill_rect(int fbIndex, int x, int  y, int w, int h, unsigned short color) {
	int i, j;
	unsigned short* fb;

	//sanity check
	if (ap.ddrMem == NULL || fbIndex < 0 || fbIndex > 1) {
		return;
	}

	fb = ap.fb[fbIndex];
	fb += (y * ap.fbWidth) + x;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			fb[i] = color;
		}
		fb += ap.fbWidth;
	}
}

// x,y - coordinates in non-rotated space
// w,h - dimensions of non-rotated image
void arvid_draw_image(int fbIndex, int x, int y, int w, int h, unsigned short* image, int rotate) {
	int j, i;
	unsigned short* ptr = ap.fb[fbIndex];
	int maxW = ap.fbWidth;
	unsigned short* src = image;
	int hm1 = h - 1;

	ptr += maxW * y + x;

	if (rotate) {
		for (j = 0; j < w; j++) {
			for (i = 0; i < h; i++) {
				int srcIndex = ((hm1 - i) * w) + j;
				ptr[i] = src[srcIndex];
			}
			ptr += maxW;
		} 
		return;
	}


	for (j = 0; j < h; j++) {
		src = image + j * w;
		for (i = 0; i < w; i++) {
			ptr[i] = *src;
			src++;
		}
		ptr += maxW;
	}
}

