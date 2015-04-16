#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "arvid.h"

/********************************************
This example sets videomode with different number of lines
and calculate framerates
*********************************************/

arvid_video_mode videoMode = arvid_320;

static void test(int lines, int seconds) {
	int res = arvid_set_video_mode(videoMode, lines);
	printf("{%i, ", lines);
	if (res == 0) {
		unsigned frame;
		usleep(seconds *1000000);
		frame = arvid_get_frame_number();
		printf(" %f},\n", (frame / (seconds * 1.0f)));
	}
}

int main(int argc , char** argv) {
	int res;
	int lines;
	int maxLines;

	if (argc > 1) {
		videoMode = (arvid_video_mode) atoi(argv[1]);
	}

	printf("using video mode=%i\n", videoMode);

	//default video mode is PAL 320x240, 50 Hz,  304 total lines
	res = arvid_init();

	//failed to initialize - bail out...
	if (res) {
		return 1;
	}

	//lines = arvid_get_video_mode_lines(5000); // 50.00 Hz
	//maxLines = arvid_get_video_mode_lines(6000); //60.00 Hz
	lines = 304;
	maxLines = 248;

	printf("please wait (test takes 42 minues) ....\n");

	test(252, 60);
	test(304, 60);
	test(279, 60);

	while (lines >= maxLines) {
		test(lines,60);
		lines--;
	}

	res = arvid_close();

	return 0;
}