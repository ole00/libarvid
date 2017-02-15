//
// Arvid software and hardware is licensed under MIT license:
//
// Copyright (c) 2015 - 2016 Marek Olejnik
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

// Horizontal resolution of 416 pixels
#define PIXELS_PER_LINE 416

//pixel cycles: 2 * 4 + 1
#define PASSIVE_CYCLES_PER_PIXEL 2
#define PASSIVE_CYCLE_PER_PIXEL_MOD1
//#define PASSIVE_CYCLE_PER_PIXEL_MOD2
//#define PASSIVE_CYCLE_PER_PIXEL_MOD3

#define ASYMETRIC_PIXELS_4

//delay cycles:  64 * 4 + 3
#define LINE_END_DELAY 64
#define LINE_END_DELAY_MOD1
#define LINE_END_DELAY_MOD2
#define LINE_END_DELAY_MOD3

#include "pruvideo_main.p"


