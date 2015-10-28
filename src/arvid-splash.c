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

/* Arvid - show splash screen */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "zlib.h"
#include "arvid.h"
#include "libarvid.h"
#include "png.h"

#if 0
//8 x 8 tiles
static unsigned char tiles[][64] = {
		//digit 0
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 1
		{
				0, 0, 0, 0, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 3, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 2
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				3, 3, 3, 3, 3, 3, 3, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 3
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 4
		{
				0, 0, 0, 3, 3, 3, 3, 0,
				0, 0, 3, 3, 0, 3, 3, 0,
				0, 3, 3, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 3, 3, 3, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 5
		{
				3, 3, 3, 3, 3, 3, 3, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,

		},
		//digit 6
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				3, 3, 0, 0, 0, 0, 0, 0,
				3, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,

		},
		//digit 7
		{
				3, 3, 3, 3, 3, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 3, 3, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
		},
		//digit 8
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,

		},
		//digit 9
		{
				0, 3, 3, 3, 3, 3, 0, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				3, 3, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 0, 0, 0, 0, 3, 3, 0,
				0, 3, 3, 3, 3, 3, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,

		},
		//dot character
		{
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 3, 3, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0,

		},

};


#endif
unsigned short* fb[2];
unsigned int* pngData;
int logoW, logoH;

static int readPng(char* fileName) {
	png_structp png;
	png_infop info;
	int depth, type;
	png_byte* buffers[480];
	int i;

	logoW = logoH = 0;

	FILE* f = fopen(fileName, "rb");

	if (!f) {
		printf("failed to read file: %s\n", fileName);
		return -1;
	}

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	if (png == NULL || info == NULL) {
		printf("failed to initialise libpng\n");
		return -1;
	}
	//error handler
	if (setjmp(png_jmpbuf(png))) {
		printf("failed to read png file %s \n", fileName);
		fclose(f);
		return -1;
	}

	png_init_io(png, f);
	png_read_info(png, info);
	logoW = png_get_image_width(png, info);
	logoH = png_get_image_height(png, info);
	depth = png_get_bit_depth(png, info);
	type = png_get_color_type(png, info);

	if (logoW > 640 || logoH > 480) {
		printf("image is too big (max 640x480)! w=%i, h=%i\n", logoW, logoH);
		fclose(f);
		return -1;
	}
	if (depth != 8 || type != PNG_COLOR_TYPE_RGBA) {
		printf("wrong image type (ARGB only)!");
		fclose(f);
		return -1;
	}

	pngData = (unsigned int*) malloc (logoW * logoH * 4);
	if (pngData == NULL) {
		printf ("failed to allocate png immage buffer\n");
		fclose (f);
		return -1;
	}
	for (i = 0; i < logoH; i++) {
		buffers[i] = (png_byte*)(pngData + (logoW * i));
	}
	png_read_image(png, buffers);
	fclose(f);
	return 0;
}

static void fillRect(int x, int y,int w, int h, unsigned short color, int frameIndex) {
	int j, i;
	unsigned short* ptr = fb[frameIndex];
	int maxW = arvid_get_width();

	ptr += maxW * y + x;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			ptr[i] = color;
		}
		ptr += maxW;
	}
}

static unsigned short getColor(unsigned int pixel) {
	int B = (pixel >> 16) & 0xFF;
	int G = (pixel >> 8) & 0xFF;
	int R = pixel & 0xFF;
	return COLOR(R, G, B);
}

static void convertLogo() {
	int max = logoW * logoH;
	unsigned int* src = pngData;
	unsigned short* dst = (unsigned short*) pngData;
	int i;

	for (i = 0; i < max; i++) {
		*dst = getColor(*src);
		dst++;
		src++;
	}
}

// x,y - coordinates in non-rotated space
// w,h - dimensions of non-rotated image
static void drawImage(int x, int y, int w, int h, unsigned short* image, int frameIndex, int rotate) {
	int j, i;
	unsigned short* ptr = fb[frameIndex];
	int maxW = arvid_get_width();
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

static void drawLogo() {
	int frame;
	int x, y;
	int w, h;
	unsigned short color;
	int buttons;
	int rotate;

	fb[0] = arvid_get_frame_buffer(0);
	fb[1] = arvid_get_frame_buffer(1);

	w = arvid_get_width();
	h = arvid_get_height();

	buttons = arvid_get_button_state();
	rotate = buttons & ARVID_TATE_SWITCH;
	//center the logo
	if (rotate) {
		x = (w - logoH) / 2;
		y = (h - logoW) / 2;
	} else 
	{
		x = (w - logoW) / 2;
		y = (h - logoH) / 2;
	}
	//use first pixel as the background color
	color = *(unsigned short *) pngData;


	frame = arvid_wait_for_vsync();

	fillRect(0,0,w,h, color, frame & 1);
	fillRect(0,0,w,h, color, 1-(frame&1));

	//make sure logo fits
	if (x >=0 && y >= 0) {
		drawImage(x, y, logoW, logoH, (unsigned short*) pngData, frame & 1, rotate);
		drawImage(x, y, logoW, logoH, (unsigned short*) pngData, 1- (frame & 1), rotate);
	}

}

int main(int argc, char**argv)
{
	int readResult;

	if (argc > 1) {
		readResult = readPng(argv[1]);
	} else {
		readResult = readPng("logo.png");
	}
	if (readResult < 0) {
		return -1;
	}
	convertLogo(); //convert ARGB8888 to RGB555

	if (arvid_init() != 0) {
		return -1;
	}

	drawLogo();

	return 0;
}
