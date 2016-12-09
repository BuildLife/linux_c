/*Basic lib*/
#define _BSD_SOURCE
#define _USE_BSD 
#include <stdio.h>
#include <stdlib.h>

/*time lib*/
#include <time.h>
#include <unistd.h>

/*Variable type*/
//#include <bsd/string.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <memory.h>

/*socket include*/
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>


#include <pcap.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/if_ether.h>

/*For Ctrl+c sending a signal liberary*/
#include <signal.h>
#include <time.h>

typedef struct S_ETH_HEADER
{
	unsigned char dmac[6];  // 6 Byte
	unsigned char smac[6];  // 6 Byte
	unsigned short type;
	unsigned short tpid; 	// 2 Byte
	unsigned short vid; 	// 2 Byte
}__attribute__((__packed__)) eth_header;

typedef struct S_IP_HEADER
{
	unsigned char ip_v:4, ip_hl:4; /*this means that each member is 4 bits*/
	unsigned char ip_tos;
	unsigned short ip_len; // value : 28 -> this means ip header + udp header length
	unsigned short ip_id; //identification
	unsigned short ip_off;
	unsigned char ip_ttl;
	unsigned char ip_p; //protocol -> tcp : 6 , udp : 17
	unsigned short ip_sum;
	unsigned int ip_src;	//4 Byte
	unsigned int ip_dst;	//4 Byte
}__attribute__((__packed__)) ip_header;

#define MAC_ADDRSTRLEN 2*6+5+1

/*Ethernet send port and receive port*/
char *LAN_port = "eth14";
char *WAN_port = "eth2";

/*use for fpga from LAN Port to WAN Port*/
char *SendBuf = {0};
char *SendpktcBuf = {0};
char *ReceiveBuf = {0};

/*Change DVGM or SVGM MODE*/
char *ChangeMode = "default";

/*Change Send docsis or pktc packet*/
char *DHCPBufMode = "default";

/*Set send times and VID Range*/
unsigned short Start_VID = 0;
int Running_Times = 0, KeepRunning = 0;

/*Record Compare data False times and True times*/
int CompareFalseTimes = 0, CompareTrueTimes = 0;

/*Record send lose packet*/
int LosePacket = 0;

/*Random Sending data*/
int Random_send = 0;



/*Pthread lock*/
//pthread_mutex_t pcap_mutex = PTHREAD_MUTEX_INITIALIZER;


/*2293 0x08 0x2D*/
/*mac default 0x00 0x1c 0x7b 0x11 0x11 0x12*/
/*DHCP Discover buffer : docsis buffer*/
char DHCPdocsisBuf[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1c, 
	0x7b, 0x11, 0x00, 0x00, 0x81, 0x00, 0x08, 0x01, 0x08, 0x00, 0x45, 0x00, 
	0x02, 0x18, 0xde, 0xad, 0x00, 0x00, 0x80, 0x11, 
	0x5a, 0x28, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
	0xff, 0xff, 0x00, 0x44, 0x00, 0x43, 0x02, 0x04, 
	0xcd, 0x21, 0x01, 0x01, 0x06, 0x00, 0x29, 0xd9, 
	0xaa, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 
	0x7b, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 
	0x53, 0x63, 0x35, 0x01, 0x01, 0x37, 0x09, 0x01, 
	0x02, 0x03, 0x04, 0x07, 0x06, 0x64, 0x7a, 0x7d, 
	0x3c, 0x0a, 0x64, 0x6f, 0x63, 0x73, 0x69, 0x73, 
	0x33, 0x2e, 0x30, 0x3a, 0x7d, 0x88, 0x00, 0x00, 
	0x11, 0x8b, 0x83, 0x01, 0x01, 0x02, 0x05, 0x7e, 
	0x01, 0x01, 0x01, 0x02, 0x01, 0x03, 0x03, 0x01, 
	0x01, 0x04, 0x01, 0x01, 0x05, 0x01, 0x01, 0x06, 
	0x01, 0x01, 0x07, 0x01, 0x0f, 0x08, 0x01, 0x10, 
	0x09, 0x01, 0x03, 0x0a, 0x01, 0x01, 0x0b, 0x01, 
	0x18, 0x0c, 0x01, 0x01, 0x0d, 0x02, 0x00, 0x40, 
	0x0e, 0x02, 0x00, 0x10, 0x0f, 0x01, 0x01, 0x10, 
	0x04, 0x00, 0x00, 0x00, 0x04, 0x11, 0x01, 0x00, 
	0x14, 0x01, 0x00, 0x15, 0x01, 0x3f, 0x16, 0x01, 
	0x01, 0x17, 0x01, 0x01, 0x18, 0x01, 0x04, 0x19, 
	0x01, 0x04, 0x1a, 0x01, 0x04, 0x1b, 0x01, 0x20, 
	0x1c, 0x01, 0x02, 0x1d, 0x01, 0x08, 0x1e, 0x01, 
	0x20, 0x1f, 0x01, 0x10, 0x20, 0x01, 0x10, 0x21, 
	0x01, 0x02, 0x22, 0x01, 0x01, 0x23, 0x01, 0x00, 
	0x24, 0x01, 0x00, 0x25, 0x01, 0x01, 0x26, 0x02, 
	0x00, 0xff, 0x27, 0x01, 0x00, 0x28, 0x01, 0x00, 
	0x2c, 0x04, 0x00, 0x00, 0x00, 0x01, 0x2b, 0x50, 
	0x02, 0x03, 0x45, 0x43, 0x4d, 0x03, 0x08, 0x45, 
	0x43, 0x4d, 0x3a, 0x45, 0x4d, 0x54, 0x41, 0x04, 
	0x00, 0x05, 0x04, 0x56, 0x31, 0x2e, 0x30, 0x06, 
	0x09, 0x35, 0x2e, 0x35, 0x2e, 0x31, 0x30, 0x6d, 
	0x70, 0x35, 0x07, 0x0a, 0x32, 0x2e, 0x34, 0x2e, 
	0x30, 0x62, 0x65, 0x74, 0x61, 0x33, 0x08, 0x06, 
	0x30, 0x30, 0x31, 0x30, 0x31, 0x38, 0x09, 0x0b, 
	0x42, 0x43, 0x4d, 0x39, 0x33, 0x33, 0x38, 0x33, 
	0x56, 0x43, 0x4d, 0x0a, 0x08, 0x42, 0x72, 0x6f, 
	0x61, 0x64, 0x63, 0x6f, 0x6d, 0xfe, 0x01, 0x08, 
	0x3d, 0x0f, 0xff, 0x7b, 0x11, 0x11, 0x12, 0x00, 
	0x03, 0x00, 0x01, 0x00, 0x1c, 0x7b, 0x11, 0x11, 
	0x12, 0x39, 0x02, 0x05, 0xdc, 0xff };


/*mac default : 0x00 0x1c 0x7b 0x22 0x22 0x24*/
/*DHCP Discover buffer : pktc buffer*/
char DHCPpktcBuf[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1c, 
0x7b, 0x22, 0x00, 0x00, 0x81, 0x00, 0x08, 0x01, 
0x08, 0x00, 0x45, 0x00, 0x02, 0x07, 0xde, 0xad, 
0x00, 0x00, 0x80, 0x11, 0x5a, 0x39, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x44, 
0x00, 0x43, 0x01, 0xf3, 0x81, 0xf6, 0x01, 0x01, 
0x06, 0x00, 0x4f, 0x8a, 0xe6, 0x24, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x1c, 0x7b, 0x22, 0x22, 0x23, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x63, 0x82, 0x53, 0x63, 0x35, 0x01, 
0x01, 0x37, 0x09, 0x01, 0x02, 0x03, 0x06, 0x07, 
0x0c, 0x0f, 0x36, 0x7a, 0x3c, 0x7a, 0x70, 0x6b, 
0x74, 0x63, 0x31, 0x2e, 0x35, 0x3a, 0x30, 0x35, 
0x33, 0x37, 0x30, 0x31, 0x30, 0x31, 0x30, 0x31, 
0x30, 0x32, 0x30, 0x31, 0x30, 0x32, 0x30, 0x39, 
0x30, 0x31, 0x30, 0x31, 0x30, 0x62, 0x30, 0x35, 
0x30, 0x36, 0x30, 0x39, 0x30, 0x65, 0x30, 0x64, 
0x30, 0x66, 0x30, 0x63, 0x30, 0x31, 0x30, 0x31, 
0x30, 0x64, 0x30, 0x31, 0x30, 0x31, 0x30, 0x66, 
0x30, 0x31, 0x30, 0x31, 0x31, 0x30, 0x30, 0x31, 
0x30, 0x39, 0x31, 0x32, 0x30, 0x32, 0x30, 0x30, 
0x30, 0x37, 0x31, 0x33, 0x30, 0x31, 0x30, 0x31, 
0x31, 0x34, 0x30, 0x31, 0x30, 0x31, 0x31, 0x35, 
0x30, 0x31, 0x30, 0x31, 0x31, 0x36, 0x30, 0x31, 
0x30, 0x31, 0x31, 0x37, 0x30, 0x33, 0x30, 0x32, 
0x30, 0x30, 0x33, 0x66, 0x31, 0x38, 0x30, 0x31, 
0x30, 0x30, 0x31, 0x39, 0x30, 0x31, 0x30, 0x30, 
0x2b, 0x59, 0x02, 0x04, 0x45, 0x4d, 0x54, 0x41, 
0x04, 0x06, 0x31, 0x31, 0x31, 0x31, 0x31, 0x32, 
0x05, 0x04, 0x56, 0x31, 0x2e, 0x30, 0x06, 0x0d, 
0x33, 0x2e, 0x39, 0x2e, 0x32, 0x31, 0x2e, 0x31, 
0x36, 0x6d, 0x70, 0x35, 0x20, 0x07, 0x0a, 0x32, 
0x2e, 0x34, 0x2e, 0x30, 0x62, 0x65, 0x74, 0x61, 
0x33, 0x08, 0x03, 0x00, 0x1c, 0x7b, 0x09, 0x0b, 
0x42, 0x43, 0x4d, 0x39, 0x33, 0x33, 0x38, 0x33, 
0x56, 0x43, 0x4d, 0x0a, 0x08, 0x42, 0x72, 0x6f, 
0x61, 0x64, 0x63, 0x6f, 0x6d, 0x1f, 0x06, 0x00, 
0x1c, 0x7b, 0x22, 0x22, 0x23, 0x20, 0x04, 0xd3, 
0x54, 0xb6, 0x5c, 0x3d, 0x0f, 0xff, 0x7b, 0x22, 
0x22, 0x23, 0x00, 0x03, 0x00, 0x01, 0x00, 0x1c, 
0x7b, 0x22, 0x22, 0x23, 0x39, 0x02, 0x05, 0xdc, 
0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x40, 0x83, 0x49, 0x25, 0xba, 0x7f, 0x00, 0x00, 
0x80, 0x81, 0x49, 0x25, 0xba, 0x7f, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x30, 0x30, 0x3a, 0x31, 0x63, 0x3a, 0x37, 0x62, 
0x3a, 0x31, 0x31, 0x3a, 0x31, 0x31, 0x3a, 0x31, 
0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0xe7, 0x8d, 0x24, 0xba, 0x7f, 0x00, 0x00, 
0x00, 0xf7, 0x0d, 0x25, 0xba, 0x7f, 0x00, 0x00, 
0x54, 0x00, 0x8c, 0x1f, 0xba, 0x7f, 0x00, 0x00, 
0x20, 0x0d, 0x01, 0x18, 0xba, 0x7f, 0x00, 0x00, 
0x30, 0xc8, 0xfd, 0x01, 0x00, 0x00, 0x00, 0x00, 
0x10, 0xc0, 0xfd, 0x01, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };



/*DHCP docsis offer packet*/
/*CM IP : 192.168.10.32*/
char DHCPdocsisBuf_offer[] = {
0x00, 0x1c, 0x7b, 0x11, 0x00, 0x00, 0xc4, 0x12, 
0xf5, 0x30, 0x92, 0xf9, 0x08, 0x00, 0x45, 0x10, 
0x01, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 
0xa3, 0xef, 0xc0, 0xa8, 0x0a, 0x02, 0xc0, 0xa8, 
0x0a, 0x20, 0x00, 0x43, 0x00, 0x44, 0x01, 0x67, 
0x33, 0x7d, 0x02, 0x01, 0x06, 0x00, 0x6c, 0xbc, 
0x59, 0xa4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc0, 0xa8, 0x0a, 0x20, 0xc0, 0xa8, 
0x0a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 
0x7b, 0x11, 0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x30, 
0x6b, 0x2e, 0x63, 0x66, 0x67, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 
0x53, 0x63, 0x35, 0x01, 0x02, 0x36, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x33, 0x04, 0x00, 0x01, 0x51, 
0x80, 0x02, 0x04, 0x00, 0x00, 0x70, 0x80, 0x03, 
0x04, 0xc0, 0xa8, 0x0a, 0x01, 0x04, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x06, 0x08, 0xa8, 0x5f, 0x01, 
0x01, 0xc0, 0xa8, 0x0a, 0x02, 0x07, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x0f, 0x03, 0x6c, 0x61, 0x62, 
0x3a, 0x04, 0x00, 0x00, 0xa8, 0xc0, 0x3b, 0x04, 
0x00, 0x01, 0x27, 0x50, 0x7a, 0x24, 0x01, 0x04, 
0xc0, 0xa8, 0x0a, 0x02, 0x03, 0x11, 0x00, 0x0a, 
0x73, 0x65, 0x72, 0x76, 0x65, 0x72, 0x32, 0x30, 
0x30, 0x38, 0x03, 0x6c, 0x61, 0x62, 0x00, 0x06, 
0x09, 0x05, 0x42, 0x41, 0x53, 0x49, 0x43, 0x01, 
0x31, 0x00, 0x01, 0x04, 0xff, 0xff, 0xff, 0x00, 
0xff };


/*DHCP pktc offer packet*/
/*EMTA IP : 192.168.10.132*/
char DHCPpktcBuf_offer[] = {
0x00, 0x1c, 0x7b, 0x22, 0x00, 0x00, 0xc4, 0x12, 
0xf5, 0x30, 0x92, 0xf9, 0x08, 0x00, 0x45, 0x10, 
0x01, 0x85, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 
0xa3, 0x81, 0xc0, 0xa8, 0x0a, 0x02, 0xc0, 0xa8, 
0x0a, 0x84, 0x00, 0x43, 0x00, 0x44, 0x01, 0x71, 
0x0a, 0x8a, 0x02, 0x01, 0x06, 0x00, 0x6c, 0xbc, 
0x59, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xc0, 0xa8, 0x0a, 0x84, 0xc0, 0xa8, 
0x0a, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 
0x7b, 0x22, 0x22, 0x23, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x75, 
0x73, 0x69, 0x70, 0x5f, 0x34, 0x30, 0x30, 0x30, 
0x2e, 0x62, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x63, 0x82, 
0x53, 0x63, 0x35, 0x01, 0x02, 0x36, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x33, 0x04, 0x00, 0x01, 0x51, 
0x80, 0x02, 0x04, 0x00, 0x00, 0x70, 0x80, 0x03, 
0x04, 0xc0, 0xa8, 0x0a, 0x01, 0x04, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x06, 0x08, 0xa8, 0x5f, 0x01, 
0x01, 0xc0, 0xa8, 0x0a, 0x02, 0x07, 0x04, 0xc0, 
0xa8, 0x0a, 0x02, 0x0c, 0x08, 0x65, 0x6e, 0x74, 
0x61, 0x34, 0x30, 0x30, 0x30, 0x0f, 0x03, 0x6c, 
0x61, 0x62, 0x3a, 0x04, 0x00, 0x00, 0xa8, 0xc0, 
0x3b, 0x04, 0x00, 0x01, 0x27, 0x50, 0x7a, 0x24, 
0x01, 0x04, 0xc0, 0xa8, 0x0a, 0x02, 0x03, 0x11, 
0x00, 0x0a, 0x73, 0x65, 0x72, 0x76, 0x65, 0x72, 
0x32, 0x30, 0x30, 0x38, 0x03, 0x6c, 0x61, 0x62, 
0x00, 0x06, 0x09, 0x05, 0x42, 0x41, 0x53, 0x49, 
0x43, 0x01, 0x31, 0x00, 0x01, 0x04, 0xff, 0xff, 
0xff, 0x00, 0xff };


int StopLoopRunning = 0;

void Signal_Stophandler()
{
	StopLoopRunning = 1;
}

void Menu(char *mode)
{
	if(mode == "default")
	{
		printf("******** USER MENU ********\n");
		printf("1.DVGM MODE\n");
		printf("2.SVGM MODE\n");
		printf("3.Exit this Process\n");
		printf("Ctrl+C --> leave the sending loop\n");
	}
	else if(mode == "DVGM" || mode == "SVGM")
	{
		printf("Enter Send Times or Enter 0 keep sending loop:\n");
		printf("Sending Times : ");
		scanf("%d",&Running_Times);
		if(Running_Times == 0)
			KeepRunning = 1;
		printf("Enter Start VID(limit : 2292):\n");
		printf("VID : ");
		scanf("%u",&Start_VID);
		if(Start_VID >= 2049 && Start_VID < 2293)
		{		
			DHCPdocsisBuf[14] = (Start_VID >> 8) & 0xff;
			DHCPdocsisBuf[15] = (Start_VID) & 0xff;
			DHCPpktcBuf[14] = (Start_VID >> 8) & 0xff;
			DHCPpktcBuf[15] = (Start_VID) & 0xff;
		}
	}
	printf("***************************\n");
}

//transformation mac address
char *mac_ntoa(unsigned char *mac_d)
{
	static char MACSTR[MAC_ADDRSTRLEN];
	snprintf(MACSTR, sizeof(MACSTR), "%02x:%02x:%02x:%02x:%02x:%02x", mac_d[0], mac_d[1], mac_d[2], mac_d[3], mac_d[4], mac_d[5]);

	return MACSTR;
}

/*DHCP SVGM Mode*/
void SVGM_Mode(u_int32_t length, const u_int8_t *content, eth_header *LAN_docsis, eth_header *LAN_pktc)
{
	/********************************* For LAN buffer **************************************/
	//LAN : set & get source mac and destination mac
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;

	//LAN : set mac 
	strlcpy(LAN_docsis_dstmac, mac_ntoa(LAN_docsis -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, mac_ntoa(LAN_docsis -> smac), sizeof(LAN_docsis_srcmac));
	strlcpy(LAN_pktc_dstmac, mac_ntoa(LAN_pktc -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, mac_ntoa(LAN_pktc -> smac), sizeof(LAN_pktc_srcmac));

	//LAN : Ethernet type
	LAN_docsis_type = ntohs(LAN_docsis -> type);
	LAN_pktc_type = ntohs(LAN_pktc -> type);

	//LAN : TPID
	LAN_docsis_TPID = ntohs(LAN_docsis -> tpid);
	LAN_pktc_TPID = ntohs(LAN_pktc -> tpid);
	/****************************************************************************************/

	/********************************* For WAN buffer ***************************************/
	eth_header *WAN_ethhdr = (eth_header*)content;

	//WAN : set & get source mac and destination mac
	char WAN_dstmac[MAC_ADDRSTRLEN] = {}, WAN_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t WAN_type;
	u_int16_t WAN_TPID;

	//WAN : set mac 
	strlcpy(WAN_dstmac, mac_ntoa(WAN_ethhdr -> dmac), sizeof(WAN_dstmac));
	strlcpy(WAN_srcmac, mac_ntoa(WAN_ethhdr -> smac), sizeof(WAN_srcmac));
	//WAN : Ethernet type
	WAN_type = ntohs(WAN_ethhdr -> type);

	//WAN : TPID
	WAN_TPID = ntohs(WAN_ethhdr -> tpid);
	/****************************************************************************************/
	
		printf("------------------------------- SVGM Mode ---------------------------------------\n");
		printf("---------------- LAN Port ---------------- | ------------- WAN Port -------------\n");
		printf("---------------- %s ---------------- | --------------- %s --------------\n",LAN_port,WAN_port);
	if(DHCPBufMode == "docsis")
	{
		printf("Destination 	: %17s        |   %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n", LAN_docsis_type, WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
		printf("                |   %c%c%c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308],content[309],content[310]);

		printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_docsis_TPID,WAN_TPID);
	}
	else if(DHCPBufMode == "pktc")
	{
		printf("Destination 	: %17s        |   %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_pktc_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		

		printf("                  |   %c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308]);

		printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_pktc_TPID,WAN_TPID);
		
	}
		/*Send buffer to ip structure*/
		dump_ip((ip_header*)(content + sizeof(eth_header)));
}


/*DHCP DVGM Mode*/
void DVGM_Mode(u_int32_t length, const u_int8_t *content, eth_header *LAN_docsis, eth_header *LAN_pktc)
{
	/********************************* For LAN buffer **************************************/
	//LAN : set & get source mac and destination mac
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;

	//LAN : set mac 
	strlcpy(LAN_docsis_dstmac, mac_ntoa(LAN_docsis -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, mac_ntoa(LAN_docsis -> smac), sizeof(LAN_docsis_srcmac));
	strlcpy(LAN_pktc_dstmac, mac_ntoa(LAN_pktc -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, mac_ntoa(LAN_pktc -> smac), sizeof(LAN_pktc_srcmac));

	//LAN : Ethernet type
	LAN_docsis_type = ntohs(LAN_docsis -> type);
	LAN_pktc_type = ntohs(LAN_pktc -> type);

	//LAN : TPID
	LAN_docsis_TPID = ntohs(LAN_docsis -> tpid);
	LAN_pktc_TPID = ntohs(LAN_pktc -> tpid);
	/****************************************************************************************/

	/********************************* For WAN buffer ***************************************/
	eth_header *WAN_ethhdr = (eth_header*)content;

	//WAN : set & get source mac and destination mac
	char WAN_dstmac[MAC_ADDRSTRLEN] = {}, WAN_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t WAN_type;
	u_int16_t WAN_TPID;

	//WAN : set mac 
	strlcpy(WAN_dstmac, mac_ntoa(WAN_ethhdr -> dmac), sizeof(WAN_dstmac));
	strlcpy(WAN_srcmac, mac_ntoa(WAN_ethhdr -> smac), sizeof(WAN_srcmac));
	//WAN : Ethernet type
	WAN_type = ntohs(WAN_ethhdr -> type);

	/****************************************************************************************/

	
		printf("------------------------------- DVGM Mode ---------------------------------------\n");
		printf("---------------- LAN Port ---------------- | ------------- WAN Port -------------\n");
		printf("---------------- %s ---------------- | ------------ %s ------------\n",LAN_port,WAN_port);
	if(DHCPBufMode == "docsis")
	{
		printf("Destination 	: %17s        |   %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_docsis_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
		printf("                |   %c%c%c%c%c%c%c%c%c\n",content[298],content[299],content[300],content[301],content[302],content[303],content[304],content[305],content[306]);

		printf("802.1Q Virtual LAN ID : %u\n",LAN_docsis_TPID);
	}
	else if(DHCPBufMode == "pktc")
	{
	
		printf("Destination 	: %17s        |   %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_pktc_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		

		printf("                  |   %c%c%c%c%c%c%c\n", content[298], content[299], content[300], content[301], content[302], content[303], content[304]);

		printf("802.1Q Virtual LAN ID : %u\n", LAN_pktc_TPID);
	}
		
	/*Send buffer to ip structure*/
	dump_ip((ip_header*)(content + sizeof(eth_header) - 4));
}


void dump_ip(ip_header *ipv4)
{
		int compare_num = 0;
		int i = 0;
		int x = 0;
		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			ReceiveBuf = (char*)ipv4;

			if(DHCPBufMode == "docsis")
			{
				for(;i<sizeof(DHCPdocsisBuf);i++)
				{
					if(SendBuf[i] != ReceiveBuf[i])
					{
						compare_num += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(;x<sizeof(DHCPpktcBuf);x++)
				{
					if(SendpktcBuf[x] != ReceiveBuf[x])
					{
						compare_num += 1;
					}
				}
			}
			
			if(compare_num > 0)
			{
				CompareFalseTimes += 1;
			}
			else
				CompareTrueTimes += 1;

			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf(" '%s' Compare data --------> %s\n", DHCPBufMode,!compare_num ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
			memset(ReceiveBuf,0,1024);
		}
}


void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{
	/*For LAN docsis buffer*/
	eth_header *LAN_docsis_ethhdr = (eth_header*)DHCPdocsisBuf;

	/*For LAN pktc buffer*/
	eth_header *LAN_pktc_ethhdr = (eth_header*)DHCPpktcBuf;


	//check buffer length have enough normal ethernet buffer
	if( header -> caplen < sizeof(ip_header) + sizeof(struct ether_header)){
		return;
	}

	char timebuf[64];
	memset(timebuf, 0, sizeof(timebuf));
	if( ctime_r(&header -> ts.tv_sec, timebuf) == NULL)
	{
		return;
	}

	if(ChangeMode == "DVGM")
	{
		unsigned short DVGM_checksum = (bytes[18] << 8) | bytes[19];
		if(DVGM_checksum == 0xdead)
		{
			printf("Current Send Times : %s",timebuf);
			DVGM_Mode(header -> caplen, bytes, LAN_docsis_ethhdr, LAN_pktc_ethhdr);
		}
	}

	else if(ChangeMode == "SVGM")
	{
		unsigned short SVGM_checksum = (bytes[22] << 8) | bytes[23];
		if(SVGM_checksum == 0xdead)
		{
			printf("Current Send Times : %s",timebuf);
			SVGM_Mode(header -> caplen, bytes, LAN_docsis_ethhdr, LAN_pktc_ethhdr);
		}
	}

}

void read_loop()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net,mask;

	//CASTLE USEING : ubuntu 12.04
	p_read = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);

	if( p_read == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	if( pcap_compile(p_read, &bpf_p, "udp", 1, mask) == -1){
		fprintf(stderr, "pcap_comiple:%s\n", pcap_geterr(p_read));
		pcap_close(p_read);
		exit(1);
	}

	if( pcap_setfilter(p_read, &bpf_p) == -1){

		fprintf(stderr, "pcap_setfilter:%s\n", pcap_geterr(p_read));
		pcap_close(p_read);
		exit(1);
	}

	pcap_freecode(&bpf_p);

	if (pcap_loop(p_read, -1, pcap_handler_func, NULL) < 0){
		fprintf(stderr, "pcap_read:%s\n", pcap_geterr(p_read));
		pcap_close(p_read);
		exit(1);
	}

	pcap_close(p_read);
}


void MACandVIDplus()
{
	//mac
	if(((DHCPdocsisBuf[10] & 0xff ) << 8 | DHCPdocsisBuf[11] & 0xff) < 2048)
	{
		if((DHCPpktcBuf[11] & 0xff) == 255)
		{
			DHCPdocsisBuf[10] += 0x01;
			DHCPpktcBuf[10] += 0x01;

//			DHCPpktcBuf_offer[4] = DHCPpktcBuf[10];
//			DHCPdocsisBuf_offer[4] = DHCPdocsisBuf[10];

			DHCPdocsisBuf[11] = 0x00;
			DHCPpktcBuf[11] = 0x00;

//			DHCPpktcBuf_offer[5] = DHCPpktcBuf[11];
//			DHCPdocsisBuf_offer[5] = DHCPdocsisBuf[11];
		}
		else
		{
			DHCPdocsisBuf[11] += 0x01;
			DHCPpktcBuf[11] += 0x01;

//			DHCPpktcBuf_offer[5] = DHCPpktcBuf[11];
//			DHCPdocsisBuf_offer[5] = DHCPdocsisBuf[11];
		}
	}
	else
	{
		DHCPpktcBuf[10] = 0x00;
		DHCPdocsisBuf[10] = 0X00;
		DHCPpktcBuf[11] = 0x00;
		DHCPdocsisBuf[11] = 0X00;


//		DHCPpktcBuf_offer[4] = 0x00;
//		DHCPdocsisBuf_offer[4] = 0X00;
//		DHCPpktcBuf_offer[5] = 0x00;
//		DHCPdocsisBuf_offer[5] = 0X00;
		
	}

	//vlan tag
	if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 2292)
	{
		DHCPpktcBuf[15] += 0x01;
		DHCPdocsisBuf[15] += 0X01;
	}
	else
	{
		DHCPpktcBuf[14] = 0x08;
		DHCPdocsisBuf[14] = 0X08;
		DHCPpktcBuf[15] = 0x01;
		DHCPdocsisBuf[15] = 0X01;
	}
}


void Option_Receive(int D_times, char sop, pcap_t *p_lan)
{
		if(sop == '1')
			ChangeMode = "DVGM";
		else if(sop == '2')
			ChangeMode = "SVGM";

		Menu(ChangeMode);
		while(D_times < Running_Times || KeepRunning == 1)
		{
			Random_send = (rand() % 100)+1;
			D_times++;

			printf("Send Times --------------> %d\n", D_times);

			if(Random_send >= 1 && Random_send < 50)
			{
				DHCPBufMode = "docsis";
				if(pcap_sendpacket(p_lan, DHCPdocsisBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					return 1;
				}

		//		sleep(1);

				//if(pcap_sendpacket(p_wan, DHCPdocsisBuf_offer, 1024) < 0){
				//	fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
			//		return 1;
			//	}
			}
			else if(Random_send >= 50 && Random_send <= 100)
			{
				DHCPBufMode = "pktc";
				if(pcap_sendpacket(p_lan, DHCPpktcBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					return 1;
				}

		//		sleep(1);

			//	if(pcap_sendpacket(p_wan, DHCPpktcBuf_offer, 1024) < 0){
			//		fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
			//		return 1;
			//	}
			}
			sleep(1);


			/*Add mac and vid function*/
			MACandVIDplus();

			printf("\n");
			DHCPBufMode = "default";
			if(StopLoopRunning == 1)
			{
				break;
			}
		}

		printf("********************\n");
		printf("Compare Send packet and Receive packet\n");
		printf("False : %d\n",CompareFalseTimes);
		printf("True  : %d\n",CompareTrueTimes);
		printf("Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n",D_times - (CompareFalseTimes + CompareTrueTimes));
		printf("********************\n");
		printf("\n");

		/***** Init default value *****/
		CompareFalseTimes = 0;
		CompareTrueTimes = 0;
		LosePacket = 0;
		KeepRunning = 0;
		StopLoopRunning = 0;
		D_times = 0;
		Menu("default");
		/******************************/
}


//compare word to enter
char buf;

void send_packet()
{
	//calculation send dhcp times
	int DHCPtimes = 0;

	char errbuf[PCAP_ERRBUF_SIZE];

	/*LAN Port pcap send*/
	pcap_t *p_send;
	
	//CASTLE USEING : ubuntu 12.04
	p_send = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);
	
	if( p_send == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	/*WAN port pacp send*/
	//pcap_t *p_wan_send;

	//p_wan_send = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);

	//while(fgets(buf, sizeof(buf), stdin) != NULL)
	while((buf = getchar()) != NULL)
	{
		/******************************** DVGM **********************************/
		//if(!(strcmp(buf, "1")))
		if(buf == '1' || buf == '2')
		{
			Option_Receive(DHCPtimes, buf, p_send);
		}
		else if(buf == '3')
		{
			free(SendBuf);
			free(ReceiveBuf);
			free(SendpktcBuf);
			pcap_close(p_send);
			exit(1);
		}
	//	else
	//	{
			//printf("No this option\n");
	//		Menu("default");
	//	}
	}
	pcap_close(p_send);
}


/*Send Packet thread*/
pthread_t pthreadSendPacket;

/*Receive Packet thread*/
pthread_t pthreadReadLoop;

int main(int argc,char *argv[])
{
	/*signal function*/
	signal(SIGINT, Signal_Stophandler);

	if(argc == 3)
	{
		/*Set sending packet port and receive port*/
		LAN_port = argv[1];
		WAN_port = argv[2];
	}

	printf("LAN Port = %s\n", LAN_port);
	printf("WAN Port = %s\n", WAN_port);

	Menu("default");

	/*store send buf in char for compare********/
	int s = 0, eth_ft = 0;
	SendBuf = malloc(1024);
	ReceiveBuf = malloc(1024);

	for(s = 18; s < sizeof(DHCPdocsisBuf); s++)
	{
		SendBuf[eth_ft++] = DHCPdocsisBuf[s];
	}

	int s_p = 0, eth_pk = 0;
	SendpktcBuf = malloc(1024);
	for(s_p = 18; s_p < sizeof(DHCPpktcBuf); s_p++)
	{
		SendpktcBuf[eth_pk++] = DHCPpktcBuf[s_p];
	}
	/*******************************************/

	int pth_send = 0, pth_read = 0;

	pth_send = pthread_create(&pthreadSendPacket, NULL, (void*)send_packet, NULL);
	if( pth_send != 0 )
	{
		printf("Create Send Function Thread Error\n");
		printf("exit........................\n");
		exit(1);
	}

	pth_read = pthread_create(&pthreadReadLoop, NULL, (void*)read_loop, NULL);
	if( pth_read != 0 )
	{
		printf("Create Read Function Thread Error\n");
		printf("exit........................\n");
		exit(1);
	}

	pthread_join(pthreadSendPacket, NULL);
	pthread_join(pthreadReadLoop, NULL);

	free(SendBuf);
	free(SendpktcBuf);
	free(ReceiveBuf);
	return 0;
}
