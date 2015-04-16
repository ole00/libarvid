/* 
Service screen
*/

#include <stdlib.h>
#include <memory.h>

#include "arvid.h"
#include "libarvid.h"
#include "blitter.h"

extern arvid_private ap;

int arvid_show_service_screen(void) {
	unsigned short* fb;
	unsigned short* framePtr;
	int topLine;
	const unsigned short grayBright = 0x777;
	const unsigned short grayDark = 0x333;
	const unsigned short borderColor = 0x722;
	int i, j;
	int posX, posY;

	//sanity check
	ARVID_INIT_CHECK;
	if (ap.ddrMem == NULL) {
		return ARVID_ERROR_NOT_INITIALIZED;
	}

	fb = ap.fb[0];

	// top line:  3/4 of the empty space 
	topLine = ((ap.lines - ap.fbHeight) / 8) * 6;

	for (j = 0; j < ap.fbHeight; j++) {
		framePtr = &fb[ap.fbWidth * (j + topLine)];
		if ((j & 7) == 0) {
			//solid bright line
			for (i = 0; i < ap.fbWidth; i++) {	
				framePtr[i] = grayBright;
			}
		} else {
			//grid line
			for (i = 0; i < ap.fbWidth; i++) {	
				framePtr[i] = (i & 0x7) == 0 ? grayBright : grayDark;
			}
		}
	}

	//generate RGB shade pattern
	posX = (ap.fbWidth - (32 * 4)) / 2 ;
	posX = (posX >> 3) << 3; //make it 8 pixel boundary
	posY = 64 + topLine;
	for (i = 0; i < 16; i++) {
		arvid_fill_rect(0, posX,      posY + (i * 8), 32, 8, i << 8); // red
		arvid_fill_rect(0, posX + 32, posY + (i * 8), 32, 8, i << 4); // green
		arvid_fill_rect(0, posX + 64, posY + (i * 8), 32, 8, i ); // blue
		arvid_fill_rect(0, posX + 96, posY + (i * 8), 32, 8, (i << 8) | (i << 4) | i); // red
	}

	//higlight borders - top and bottom
	j = ap.fbWidth / 8;
	for (i = 0; i < j; i++) {
		arvid_fill_rect(0, 3 + i * 8, topLine + 3, 3, 3, borderColor);
		arvid_fill_rect(0, 3 + i * 8, topLine + ap.fbHeight - 5, 3, 3, borderColor);
	}


	//higlight borders - left and right
	j = ap.fbHeight / 8;
	for (i = 0; i < j; i++) {
		arvid_fill_rect(0, 3, topLine + 3 + i * 8, 3, 3, borderColor);
		arvid_fill_rect(0, ap.fbWidth - 5, topLine + 3 + i * 8, 3, 3, borderColor);
	}

	//copy framebuffer
	memcpy(ap.fb[1], ap.fb[0], 0x100000);
	return 0;
}