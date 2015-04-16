
/* Arvic UDP server */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>

#include "zlib.h"
#include "arvid.h"


unsigned char* zSrcData;
unsigned int zSrcSize;
unsigned short* zDstData;

unsigned int inflate_in(void* descriptor, unsigned char** buffer) {
	buffer = &zSrcData;
	return zSrcSize;
}

int inflate_out(void* descriptor, unsigned char* buffer, unsigned int size) {
	//printf("inflate out: size=%i\n", size);
	memcpy(descriptor, buffer, size);
	return 0;
}


int main(int argc, char**argv)
{
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	unsigned short data[8 + 4 * 1024];
	unsigned short* fb[2];
	int totalSize = sizeof(data);
	z_stream zStream;
	unsigned char* zWindow;

	zWindow = (unsigned char*) malloc(32 * 1024);
	if (zWindow == NULL) {
		printf("failed to allocate zWindow \n");
	}
	zSrcData = (unsigned char*) data;

	zStream.zalloc = Z_NULL;
	zStream.zfree = Z_NULL;
	zStream.opaque = Z_NULL;

	inflateBackInit(&zStream, 15, zWindow);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(32100);
	bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	if (arvid_init() != 0) {
		return -1;
	}
	fb[0] = arvid_get_frame_buffer(0);
	fb[1] = arvid_get_frame_buffer(1);

	arvid_show_service_screen();
	len = sizeof(cliaddr);

	while(1)
	{
		recvfrom(sockfd, (char*) data, totalSize, 0,(struct sockaddr *)&cliaddr,&len);
		//printf("command: %i\n", data[0]);
		switch (data[0]) {
			case 1: // blit 
				{
					int bufferIndex = 1 - (arvid_get_frame_number() & 1);
					//int bufferIndex = data[3];
					unsigned short size = data[1];
					unsigned short y = data[2];
					int width = arvid_get_width();
					unsigned short* dst = fb[bufferIndex] + (y * width);
					uLongf chunkSize = (32 * width) << 1;
					zSrcSize = size;


					zStream.avail_in = size;
					zStream.next_in = (void*) &data[8];
					zStream.avail_out = chunkSize;
					zStream.next_out = (void*) dst;
					inflateBack(&zStream, inflate_in, NULL, inflate_out, dst);

					//printf("  blit: size=%i, y=%i infl=%i\n", size, y, inflated);
				} break;
			case 2: // get frame number
				{
					int* result =  (int*) data;
					*result = arvid_get_frame_number();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				} break;
			case 3: //wait for vsync - returns current frame number
				{
					int* result =  (int*) data;
					arvid_wait_for_vsync();
					*result = arvid_get_frame_number();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				} break;
			case 4: //set video mode
				{
					int* result = (int*) data;
					*result = arvid_set_video_mode((arvid_video_mode) data[1], data[2]);
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				}; break;
			case 5: //get video mode lines
				{
					int* result = (int*) data;
					*result = arvid_get_video_mode_lines((arvid_video_mode) data[1], (float)( data[2] / 1000.0f));  
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				}; break;
			case 6: //get video mode refresh rate
				{
					int* result = (int*) data;
					*result = arvid_get_video_mode_refresh_rate((arvid_video_mode) data[1], data[2]) * 1000;
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				}; break;
			case 7: // get width
				{
					int* result = (int*) data;
					*result = arvid_get_width();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				}; break;
			case 8: // get height
				{
					int* result = (int*) data;
					*result = arvid_get_height();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
				}; break;
			case 11: // init
				{
					int* result = (int*) data;
					*result = arvid_init();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					//set cpu freq to stable frequency
					system("cpufreq-set -f 800MHz");
				}; break;

			case 12: // close
				{
					int* result = (int*) data;
					*result = arvid_close();
					sendto(sockfd, data, 4, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					//set cpu frequency back to default
					system("cpufreq-set -g ondemand");

				}; break;


		}
	}
}
