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
- closes arvid
*********************************************/

int main(int argc , char** argv) {
	int res;
	int lines;

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


	res = arvid_close();
	printf("example: arvid_close result=%i\n", res);

	return 0;
}