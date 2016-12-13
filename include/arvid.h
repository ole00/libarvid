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

#ifndef _ARVID_H_
#define _ARVID_H_
/*
**********************************************************
           ARVID - homebrew arcade videocard
**********************************************************
       !!! BEWARE: use at your own risk !!!!
!!!  You may damage your TV if arvid is set incorrectly!
**********************************************************

*/


#define ARVID_ERROR_NOT_INITIALIZED  (-1)
#define ARVID_ERROR_PRU_INIT_FAILED  (-2)
#define ARVID_ERROR_PRU_LOAD_FAILED  (-3)
#define ARVID_ERROR_PRU_MEMORY_MAPPING_FAILED (-4)
#define ARVID_ERROR_PRU_PERMISSION_REQUIRED  (-5)
#define ARVID_ERROR_PRU_CLOSE_FAILED (-6)
#define ARVID_ERROR_ILLEGAL_VIDEO_MODE (-7)
#define ARVID_ERROR_THREAD_FAILED (-8)
#define ARVID_ERROR_DMA_INIT_FAILED (-9)


#define ARVID_TATE_SWITCH (1 << 19)
#define ARVID_COIN_BUTTON (1 << 17)
#define ARVID_START_BUTTON (1 << 21)

#define FLAG_NO_FB_CLEAR (1 << 0)

typedef enum  {
	arvid_320 = 0,
	arvid_256, /* 1 */
	arvid_288, /* 2 */
	arvid_384, /* 3 */
	arvid_240, /* 4 */
	arvid_392, /* 5 */
	arvid_400, /* 6 */
	arvid_292, /* 7 */
	arvid_336, /* 8 */
	arvid_last_video_mode
} arvid_video_mode;


typedef struct arvid_vmode_info_s {
    unsigned short width;
    unsigned short vmode;
} arvid_vmode_info;

/**
Initializes Arvic: loads pru code, creates frame buffer mappings.
Also sets the video mode to 320x240 60Hz, black frame buffers.
returns: 0 if OK
*/
int arvid_init(void);

/**
Same as above, but with extra initialisation flags.
FLAG_NO_FB_CLEAR - will not clear frame buffers
*/
int arvid_init_ex(int initFlags);


/**
Closes arvid: stops pru code, unmaps frame buffers
return: 0 if OK
*/
int arvid_close(void);


/**
Displays service mode screen.
Usefull for testing colors and screen geometry.
*/
int arvid_show_service_screen(void);

/**
Set current video mode - resolution and framerate
mode: specifies horizontal resolution
lines: specifies vertical resolution in lines per frame.
      Also affects the refresh rate.
      Allowed values are between 262 (NTSC 60 Hz) to 
      304 (PAL 50Hz) including.
*/
int arvid_set_video_mode(arvid_video_mode mode, int lines);

/**
Calculates number of video mode lines according screen refresh rate.
Note: result values are dependent on selected videomode.
rate: values lower than 50 and higher than 61 are clamped to the bounds.
*/
int arvid_get_video_mode_lines(arvid_video_mode mode, float rate);


/**
returns refresh rate for particular videomode
*/ 
float arvid_get_video_mode_refresh_rate(arvid_video_mode mode, int lines);


/**
Enumerates available videomodes. Results are passed to vmodeInfo variable.
MaxItems is set to total number of available videomodes (even on error)
as long as maxItems is not NULL. Set maxItems to the total size of the
vmodeInfo items before calling the function so the library knows the
vmodeInfo buffer size.
The result list is sorted in descending order according the width of the 
videomode.
Returns 0 on success, -1 on error. 
*/
int arvid_enum_video_modes(arvid_vmode_info* vmodeInfo, int* maxItems);

/**
Returns the frame buffer pointer for double buffered screen.
Frame buffer pinters don't change over runtime.
Note: pixel format of the framebuffer is RGB555 that is 
      5 bits per color component, no alpha.
index: either 0 or 1
Returns NULL on error.
*/
unsigned short* arvid_get_frame_buffer(int index);


/**
Returns the size of a single framebuffer in pixels.
To get byte size muliply the result by 2 (16 bits per pixel).
The size is equal width * video lines.
Returns 0 on error.
*/
unsigned int arvid_get_frame_buffer_size(void);


/**
Blocks and waits for v-sync event.
returns the back buffer index (value 0 or 1) where it is safe to render
or negative error code.
*/
int arvid_wait_for_vsync(void);

/**
Returns bitmask of button state.
*/
int arvid_get_button_state(void);

/**
Returns current resolution width in pixels or error code.
Positive values corresponds to width of the frame buffer.
*/
int arvid_get_width(void);


/**
Returns current resolution height in pixels (lines) or error code.
Corresponds to the height of the framebuffer.
*/
int arvid_get_height(void);

/**
Returns current frame number.
*/
unsigned int arvid_get_frame_number(void);

/**
Set the line position modifier.
*/
void arvid_set_line_pos(int mod);


/**
Returns the line position modifier.
*/
int arvid_get_line_pos(void);

#endif
