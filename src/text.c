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

/* Naive text rendering functions */
#include <stdlib.h>
#include "libarvid.h"
#include "fixed_font.h"

extern arvid_private ap;

static void drawChar(int x, int  y, int charIndex, unsigned short color, int frameIndex, int rotate) {
	int  j, i;
	unsigned short* ptr;
	unsigned char* src;
	int maxW = ap.fbWidth;
	unsigned short* dst;

	//sanity check
	if ( frameIndex < 0 || frameIndex > 1 || ap.fb[frameIndex] == NULL) {
		return;
	}

	if (charIndex < 32 || charIndex > 138) {
		charIndex = '?';
	}
	charIndex -= 32;

	src = &fixed_font[charIndex * 8];

	ptr  = ap.fb[frameIndex];
	ptr += maxW * y + x;

	if (rotate) {
		for (j = 0; j < 8; j++) {
			dst = ptr + (7 - j);
			for (i = 0; i < 8; i++) {
				if (src[i]) {
					*dst = color;
				}
				dst += maxW;
			}
			src += FIXED_FONT_WIDTH;
		}
		return;
	}

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			if (src[i]) {
				ptr[i] = color;
			}
		}
		src += FIXED_FONT_WIDTH ;
		ptr += maxW;
	}
}

//naive text drawing
void arvid_draw_string(int fbIndex, const char* text, int x, int y, unsigned short color, int rotate) {
	while (*text != 0) {
		char c = *text;

		drawChar(x, y, c, color, fbIndex, rotate);
		if (rotate) {
			y += 8;
		} else {
			x += 8;
		}
		text++;
	}
}
