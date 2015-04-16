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

typedef enum  {
	arvid_320 = 0,
	arvid_256,
	arvid_288,
	arvid_384,
	arvid_last_video_mode
} arvid_video_mode;



/**
Initializes Arvic: loads pru code, creates frame buffer mappings.
Also sets the video mode to 320x240 50Hz, black frame buffers.
returns: 0 if OK
*/
int arvid_init(void);


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
Returns the frame buffer pointer for double buffered screen.
Frame buffer pinters don't change over runtime.
Note: pixel format of the framebuffer is RGB444 that is 
      4 bits per color component, no alpha.
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
Return current frame number
*/
unsigned int arvid_get_frame_number(void);


#endif