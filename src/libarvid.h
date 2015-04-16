#ifndef _LIBARVID_H_
#define _LIBARVID_H_



//internal for arvid
#define INITIAL_VIDEO_MODE arvid_384
#define INITIAL_FB_W  384
#define INITIAL_FB_H  240
#define INITIAL_FB_LINES 304

#define RATE_SIZE 57

#define ARVID_INIT_CHECK if (ap.initialized != 0xACCE5503) return ARVID_ERROR_NOT_INITIALIZED



typedef struct arvid_private_t {
	unsigned int initialized;
	unsigned int ddrAddress;
	unsigned int* ddrMem;
	unsigned int* pruMem;
	int ddrFd;

	int lines;
	int fbWidth;
	int fbHeight;

	unsigned short* fb[2];


} arvid_private;

typedef struct arvid_line_rate_t {
	unsigned short line;
	float rate;
} arvid_line_rate;

#endif