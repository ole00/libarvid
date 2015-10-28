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

/* Arvid UDP server */
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <stdio.h>
#include <memory.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "zlib.h"
#include "arvid.h"
#include "libarvid.h"

//send back up to 3 packets of the same content
#define PACKET_CNT 3


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

unsigned short* fb[2];

unsigned char* zSrcData;
unsigned int zSrcSize;
unsigned short* zDstData;

unsigned int inflate_in(void* descriptor, unsigned char** buffer) {
	printf("in called!\n");
	*buffer = zSrcData;
	return zSrcSize;
}

int inflate_out(void* descriptor, unsigned char* buffer, unsigned int size) {
	//printf("inflate out: size=%i\n", size);
	memcpy(descriptor, buffer, size);
	return 0;
}

static int checkInterfaceLink(char* name) {
	int res;
	struct ifreq request;
	struct ethtool_value data;
	int socketId = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (socketId < 0) {
		return 0;
	}
	strcpy(request.ifr_name, name);
	request.ifr_data = (char*) &data;
	data.cmd = ETHTOOL_GLINK;
	res = ioctl(socketId, SIOCETHTOOL, &request);
	close(socketId);
	if (res < 0) {
		return 0;
	}
	return !!data.data;
}

static void getIpAddress(char** result) {
	struct ifaddrs* addr;
	struct ifaddrs* root = NULL;
	char* ifName;
	int eth0Enabled = checkInterfaceLink("eth0");
	int usb0Enabled = checkInterfaceLink("usb0");

	getifaddrs(&root);
	addr = root;

	printf("eth0=%i\n", eth0Enabled);
	printf("usb0=%i\n", usb0Enabled);

	//no network
	if (eth0Enabled == 0 && usb0Enabled == 0) {
		*result = (char*) malloc(10);
		strcpy(*result, "0.0.0.0");
		return;
	}

	ifName = eth0Enabled ? "eth0" : "usb0";

	while (addr != NULL) {
		if (
			addr->ifa_addr != NULL && 
			addr->ifa_addr->sa_family == AF_INET &&
			strncmp(ifName, addr->ifa_name, 4) == 0)
		{
			struct sockaddr_in* inetAddr = (struct sockaddr_in*) addr->ifa_addr;
			*result = inet_ntoa(inetAddr->sin_addr);
			break;
		}
		addr = addr->ifa_next;
	}

	if (root != NULL) {
		freeifaddrs(root);
	}
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

static void paintTile(int x, int  y, int tileIndex, unsigned short color, int frameIndex) {
	int  j, i;
	unsigned short* ptr = fb[frameIndex];
	unsigned char* src = tiles[tileIndex];
	int maxW = arvid_get_width();

	ptr += maxW * y + x;

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			if (src[i]) {
				ptr[i] = color;
			}
		}
		src += 8;
		ptr += maxW;
	}
}

//naive number painting
static void paintString(char* text, int x, int y,unsigned short color) {
	while (*text != 0) {
		char c = *text;
		int index = c - '0';
		if (c == '.') {
			index = 10;
		}

		if (index >= 0 && index <= 10) {
			paintTile(x, y, index, color, 0);
			paintTile(x, y, index, color, 1);
			x += 8;
		}
		text++;
	}
}

int main(int argc, char**argv)
{
	int i;
	int sockfd;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t len;
	unsigned short data[8 + 16 * 1024];
	int totalSize = sizeof(data);
	z_stream zStream;
	unsigned char* zWindow;
	char* ipAddr = NULL;
	unsigned short packetId = 0x1FFF;

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


	getIpAddress(&ipAddr);
	printf("ip address = %s\n", ipAddr);

	arvid_show_service_screen();
	fb[0] = arvid_get_frame_buffer(0);
	fb[1] = arvid_get_frame_buffer(1);

	//draw ip address on screen

	if (ipAddr != NULL) {
		int textW = strlen(ipAddr) * 8;
		int posX = (arvid_get_width() - textW ) / 2;
		int posY = arvid_get_height() - 48;
		unsigned short color = COLOR(0x1F, 0x1F, 0x1F); //background color
		fillRect(posX - 2, posY - 2, textW + 4, 12, color, 0);
		fillRect(posX - 2, posY - 2, textW + 4, 12, color, 1);
		paintString(ipAddr, posX, posY, COLOR(0xA0, 0x80, 0));
	}


	len = sizeof(cliaddr);

	while(1)
	{
		recvfrom(sockfd, (char*) data, totalSize, 0,(struct sockaddr *)&cliaddr,&len);

		//Very crude mechanism how to tackle lost packets on critical commands.
		//Basically we send every critical packet X times to hope at least one
		//of them is transferred, then we ignore the duplicate packets.
		//To distinguish duplicate packets every packet has its packet id.
		//Duplicates have the same packet id.
		//We do similar thing when sending back a response - we send the
		//response X times and expect the receiver throws away any duplicates.

		//ignore duplicate packets except on blits
		if (data[0] != 1) {
			unsigned short id = data[1];
			//packet id is identical - ignore this packet
			if (id == packetId) {
				data[0] = 0; //ignore command
			} else {
				packetId = id;
			}
		}
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
					int* result =  (int*) &data[1];
					*result = arvid_get_frame_number();
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				} break;
			case 3: //wait for vsync - returns current frame number
				{
					int* result =  (int*) &data[1];
					int* button = result + 1;
					arvid_wait_for_vsync();
					*result = arvid_get_frame_number();
					*button = arvid_get_button_state();
					//printf("button = %08x \n", *button);
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 10, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				} break;
			case 4: //set video mode
				{
					int* result = (int*) &data[1];
					*result = arvid_set_video_mode((arvid_video_mode) data[2], data[3]);
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				}; break;
			case 5: //get video mode lines
				{
					int* result = (int*) &data[1];
					*result = arvid_get_video_mode_lines((arvid_video_mode) data[2], (float)( data[3] / 1000.0f));  
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				}; break;
			case 6: //get video mode refresh rate
				{
					int* result = (int*) &data[1];
					*result = arvid_get_video_mode_refresh_rate((arvid_video_mode) data[2], data[3]) * 1000;
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				}; break;
			case 7: // get width
				{
					int* result = (int*) &data[1];
					*result = arvid_get_width();
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				}; break;
			case 8: // get height
				{
					int* result = (int*) &data[1];
					*result = arvid_get_height();
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
				}; break;
			case 11: // init
				{
					int* result = (int*) &data[1];
					*result = arvid_init();
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
					//set cpu freq to stable frequency
					//system("cpufreq-set -f 800MHz");
					system("cpufreq-set -f 1000MHz");
				}; break;

			case 12: // close
				{
					int* result = (int*) &data[1];
					*result = arvid_close();
					data[0] = packetId;
					for (i = 0; i < PACKET_CNT; i++) {
						sendto(sockfd, data, 6, 0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
					}
					//set cpu frequency back to default
					system("cpufreq-set -g ondemand");
					packetId = 0x1FFF;
				}; break;


		}
	}
}
