#include <stdlib.h>
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