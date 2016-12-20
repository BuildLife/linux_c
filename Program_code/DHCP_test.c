/*Basic lib*/
#define _BSD_SOURCE
#define _USE_BSD 
#include <stdio.h>
#include <stdlib.h>

/*time lib*/
#include <time.h>
#include <unistd.h>

/*Variable type*/
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
#include <sys/wait.h>
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

/*Define use function*/
void Option_Receive(int, char);



/*Ethernet send port and receive port*/
char *LAN_port = "eth14";
char *WAN_port = "eth2";

/*use for fpga from LAN Port to WAN Port*/
char *SendBuf = {0};
char *SendpktcBuf = {0};
char *ReceiveBuf = {0};

char *SendBuf_offer = {0};
char *SendpktcBuf_offer = {0};
char *ReceiveBuf_offer = {0};

/*Change DVGM or SVGM MODE*/
char *ChangeMode = "default";

/*Change Send docsis or pktc packet*/
char *DHCPBufMode = "default";

/*Set send times and VID Range*/
unsigned short Start_VID = 0;
unsigned int Running_Times = 0, KeepRunning = 0;

/*Record Compare data False times and True times*/
int CompareFalseTimes = 0, CompareTrueTimes = 0;
int CompareFalseTimes_offer = 0, CompareTrueTimes_offer = 0;

/*Record send lose packet*/
int LosePacket = 0;

/*Random Sending data*/
int Random_send = 0;


/*Auto testing flag*/
int AutoTesting = 0;

/*Pthread lock*/
pthread_mutex_t pcap_send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pcap_read_mutex = PTHREAD_MUTEX_INITIALIZER;


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
0xff };


/*DHCP docsis offer packet*/
/*CM IP : 192.168.10.32*/
char DHCPdocsisBuf_offer[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x12, 
0xf5, 0x30, 0x92, 0xf9, 0x08, 0x00, 0x45, 0x10, 
0x01, 0x7b, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 
0xa3, 0xef, 0xc0, 0xa8, 0x0a, 0x02, 0xc0, 0xa8, 
0x0a, 0x20, 0x00, 0x43, 0x00, 0x44, 0x01, 0x67, 
0x33, 0x7d, 0x02, 0x01, 0x06, 0x00, 0x29, 0xd9, 
0xaa, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
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
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc4, 0x12, 
0xf5, 0x30, 0x92, 0xf9, 0x08, 0x00, 0x45, 0x10, 
0x01, 0x85, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 
0xa3, 0x81, 0xc0, 0xa8, 0x0a, 0x02, 0xc0, 0xa8, 
0x0a, 0x84, 0x00, 0x43, 0x00, 0x44, 0x01, 0x71, 
0x0a, 0x8a, 0x02, 0x01, 0x06, 0x00, 0x4f, 0x8a, 
0xe6, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
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


/*Ctrl+c change the values for stop read/send while*/
void Signal_Stophandler()
{
	StopLoopRunning = 1;
}

/*For test signal alarm*/
/*void SignalAlarmProcessStatus(int signo)
{
	alarm(10);
	printf("set alarm status for check\n");
	//signal(SIGALRM,SignalAlarmProcessStatus);
}*/


/*Client socket for use*/
void ThreadClientSocket()
{	
	int getvalue = 0;
	int clientfd;
	int res = 0;
	struct sockaddr_in client_addr;
	char client_buffer[128];

	/*create socket*/
	clientfd = socket(AF_INET, SOCK_STREAM, 0);
	if(clientfd < 0)
	{
		perror("Open client socket Error\n");
	}
	else
		printf("Open client socket success\n");

	/*Initialize client socket*/
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(9998);
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	/*Connet to server*/
	connect(clientfd, (struct sockaddr*)&client_addr, sizeof(client_addr));

	/*Receive message from Server controller*/
	bzero(client_buffer,128);
	for(;;)
	{
	res = recv(clientfd, client_buffer, sizeof(client_buffer), 0);

	if(res < 0)
	{
		printf("Can't receive socket buffer\n");
	}
	else
	{
		//printf("Receive from server :%d\n",client_buffer[3]&0xff);
	//	printf("Receive from server :0x%02x\n",client_buffer[1]);
	//	printf("Receive from server :0x%02x\n",client_buffer[2]);
		getvalue = (client_buffer[1] & 0xff) << 8 | client_buffer[2] & 0xff;
	//	printf("Running times :%d\n",getvalue);
	}

	if(client_buffer[0] != 0 && (client_buffer[3] == 0x01))
	{
		AutoTesting = 1;
		Running_Times = getvalue;
		Option_Receive(0, '1');
	//	fputc(getvalue,stdout);
	//	fputc('\n',stdout);
		client_buffer[3] = 0x00;
		send(clientfd, client_buffer, 4,0);
	}
	}
	close(clientfd);
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
	else if( mode == "DVGM" || mode == "SVGM" )
	{
		if(AutoTesting == 0)
		{
			printf("Enter Send Times or Enter 0 keep sending loop:\n");
			printf("Sending Times : ");
			scanf("%d",&Running_Times);
			if(Running_Times == 0)
			{
				/*Use in while loop always still Running*/
				KeepRunning = 1;

				/*It's for control stop while loop of Ctrl+c*/
				/*Avoid enter expect Ctrl+c, so in the start to set 0*/
				StopLoopRunning = 0;
			}
			printf("Enter Start VID(limit : 2292):\n");
			printf("VID : ");
			scanf("%u",&Start_VID);
			if(Start_VID >= 2049 && Start_VID < 2293)
			//if(Start_VID >= 2049 && Start_VID < 2512)
			{		
				DHCPdocsisBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPdocsisBuf[15] = (Start_VID) & 0xff;
				DHCPpktcBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPpktcBuf[15] = (Start_VID) & 0xff;
			}
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
		printf("---------------- LAN Port ================ | ============> WAN Port -------------\n");
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
		dump_ip((ip_header*)(content + sizeof(eth_header)),length - sizeof(eth_header));
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
		printf("---------------- LAN Port ================ | ============> WAN Port -------------\n");
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
	dump_ip((ip_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
}


void dump_ip(ip_header *ipv4, int length)
{
		/*for discover buffer*/
		int compare_num = 0;
		int i = 0;
		int x = 0;

		/*for offer buffer*/
		int compare_offer = 0;
		int i_offer = 0;
		int x_offer = 0;
		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			ReceiveBuf = (char*)ipv4;

			if(DHCPBufMode == "docsis")
			{
				for(;i<length;i++)
				{
					if(SendBuf[i] != ReceiveBuf[i])
					{
						compare_num += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(;x<length;x++)
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
			memset(ReceiveBuf,0,length);
			
			/*for test*/
			if((!compare_num) == 0)
			{
				StopLoopRunning = 1;
			}

		}
		else if(htons(ipv4 -> ip_sum) == 0xa381 || htons(ipv4 -> ip_sum) == 0xa3ef)
		{
			ReceiveBuf_offer = (char*)ipv4;
			if(DHCPBufMode == "docsis")
			{
				for(;i_offer<length;i_offer++)
				{
					if(SendBuf_offer[i_offer] != ReceiveBuf_offer[i_offer])
					{
						compare_offer += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(;x_offer<length;x_offer++)
				{
					if(SendpktcBuf_offer[x_offer] != ReceiveBuf_offer[x_offer])
					{
						compare_offer += 1;
					}
				}
			}
			
			if(compare_offer > 0)
			{
				CompareFalseTimes_offer += 1;
			}
			else
				CompareTrueTimes_offer += 1;

			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf(" '%s offer' Compare data --------> %s\n", DHCPBufMode,!compare_offer ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
			memset(ReceiveBuf_offer,0,length);
			/*for test*/
			/*if((!compare_offer) == 0)
			{
				StopLoopRunning = 1;
			}*/
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

		
	/*Send buffer to ip structure*/
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
	//pthread_mutex_unlock(&pcap_read_mutex);
}


/*LAN Port read Function*/
void pcap_handler_func_lan(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{
	unsigned short checksum_offer = (bytes[24] << 8) | bytes[25];
	/*For LAN docsis buffer*/
	eth_header *WAN_docsis_ethhdr_offer = (eth_header*)DHCPdocsisBuf_offer;

	/*For LAN pktc buffer*/
	eth_header *WAN_pktc_ethhdr_offer = (eth_header*)DHCPpktcBuf_offer;


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

	/********************************* For LAN buffer ***************************************/
	char WAN_docsis_dstmac_offer[MAC_ADDRSTRLEN] = {}, WAN_docsis_srcmac_offer[MAC_ADDRSTRLEN] = {};
	char WAN_pktc_dstmac_offer[MAC_ADDRSTRLEN] = {}, WAN_pktc_srcmac_offer[MAC_ADDRSTRLEN] = {};

	u_int16_t WAN_docsis_type;
	u_int16_t WAN_pktc_type;

	//LAN : set mac 
	strlcpy(WAN_docsis_dstmac_offer, mac_ntoa(WAN_docsis_ethhdr_offer -> dmac), sizeof(WAN_docsis_dstmac_offer));
	strlcpy(WAN_docsis_srcmac_offer, mac_ntoa(WAN_docsis_ethhdr_offer -> smac), sizeof(WAN_docsis_srcmac_offer));
	strlcpy(WAN_pktc_dstmac_offer, mac_ntoa(WAN_pktc_ethhdr_offer -> dmac), sizeof(WAN_pktc_dstmac_offer));
	strlcpy(WAN_pktc_srcmac_offer, mac_ntoa(WAN_pktc_ethhdr_offer -> smac), sizeof(WAN_pktc_srcmac_offer));

	//LAN : Ethernet type
	WAN_docsis_type = ntohs(WAN_docsis_ethhdr_offer -> type);
	WAN_pktc_type = ntohs(WAN_pktc_ethhdr_offer -> type);

	/****************************************************************************************/

	/********************************* For WAN buffer ***************************************/
	eth_header *LAN_ethhdr_offer = (eth_header*)bytes;

	//WAN : set & get source mac and destination mac
	char LAN_dstmac_offer[MAC_ADDRSTRLEN] = {}, LAN_srcmac_offer[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_type;

	//WAN : set mac 
	strlcpy(LAN_dstmac_offer, mac_ntoa(LAN_ethhdr_offer -> dmac), sizeof(LAN_dstmac_offer));
	strlcpy(LAN_srcmac_offer, mac_ntoa(LAN_ethhdr_offer -> smac), sizeof(LAN_srcmac_offer));
	//WAN : Ethernet type
	LAN_type = ntohs(LAN_ethhdr_offer -> type);

	/****************************************************************************************/

	char docsis_src[64], docsis_dst[64];
	char pktc_src[64], pktc_dst[64];

	ip_header *ip_docsis = (ip_header*)(bytes + sizeof(eth_header) - 4 );
	ip_header *ip_pktc = (ip_header*)(bytes + sizeof(eth_header) - 4 );

	/*offer docsis ip address*/
	inet_ntop(AF_INET, &ip_docsis -> ip_dst, docsis_dst, sizeof(docsis_dst));
	inet_ntop(AF_INET, &ip_docsis -> ip_src, docsis_src, sizeof(docsis_src));
	
	/*offer pkc ip address*/
	inet_ntop(AF_INET, &ip_pktc -> ip_dst, pktc_dst, sizeof(pktc_dst));
	inet_ntop(AF_INET, &ip_pktc -> ip_src, pktc_src, sizeof(pktc_src));

	if(checksum_offer == 0xa381 || checksum_offer == 0xa3ef)
	{
		printf("Current Send Times : %s",timebuf);
		printf("------------------------------- DHCP OFFER --------------------------------------\n");
		printf("---------------- LAN Port <================ | ============ WAN Port -------------\n");
		printf("---------------- %s ---------------- | ------------ %s ------------\n",LAN_port,WAN_port);
		if(DHCPBufMode == "docsis")
		{
			printf("------------------------------- DOCSIS OFFER --------------------------------------\n");
			printf("Destination 	: %17s        |   %17s\n",WAN_docsis_dstmac_offer, LAN_dstmac_offer);
	
			printf("Source      	: %17s        |   %17s\n",WAN_docsis_srcmac_offer, LAN_srcmac_offer);
			
			printf("Destination IP  : %s            |   %u.%u.%u.%u\n",docsis_dst, DHCPdocsisBuf_offer[30] & 0xff, DHCPdocsisBuf_offer[31] & 0xff, DHCPdocsisBuf_offer[32] & 0xff,DHCPdocsisBuf_offer[33] & 0xff);
			printf("Source IP       : %s             |   %u.%u.%u.%u\n",docsis_src, DHCPdocsisBuf_offer[26] & 0xff, DHCPdocsisBuf_offer[27] & 0xff, DHCPdocsisBuf_offer[28] & 0xff,DHCPdocsisBuf_offer[29] & 0xff);
			printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",WAN_docsis_type,LAN_type);
		
		}
		else if(DHCPBufMode == "pktc")
		{
			printf("------------------------------- PKTC OFFER --------------------------------------\n");
			printf("Destination 	: %17s        |   %17s\n",WAN_pktc_dstmac_offer, LAN_dstmac_offer);
	
			printf("Source      	: %17s        |   %17s\n",WAN_pktc_srcmac_offer, LAN_srcmac_offer);
			printf("Destination IP  : %s           |   %u.%u.%u.%u\n",pktc_dst, DHCPpktcBuf_offer[30] & 0xff, DHCPpktcBuf_offer[31] & 0xff, DHCPpktcBuf_offer[32] & 0xff,DHCPpktcBuf_offer[33] & 0xff);
			printf("Source IP       : %s             |   %u.%u.%u.%u\n",pktc_src, DHCPpktcBuf_offer[26] & 0xff, DHCPpktcBuf_offer[27] & 0xff, DHCPpktcBuf_offer[28] & 0xff,DHCPpktcBuf_offer[29] & 0xff);
	
			printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",WAN_pktc_type,LAN_type);
		
		}

		dump_ip((ip_header*)(bytes + sizeof(eth_header) - 4), header -> caplen - sizeof(eth_header) - 4);
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

void read_loop_lan()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net,mask;

	//read lan port
	pcap_t *p_read_lan;
	p_read_lan = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);

	if( p_read_lan == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	if( pcap_compile(p_read_lan, &bpf_p, "udp", 1, mask) == -1){
		fprintf(stderr, "pcap_comiple:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	if( pcap_setfilter(p_read_lan, &bpf_p) == -1){

		fprintf(stderr, "pcap_setfilter:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	pcap_freecode(&bpf_p);

	if (pcap_loop(p_read_lan, -1, pcap_handler_func_lan, NULL) < 0){
		fprintf(stderr, "pcap_read:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	pcap_close(p_read_lan);
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

		//	DHCPpktcBuf_offer[4] = DHCPpktcBuf[10];
		//	DHCPdocsisBuf_offer[4] = DHCPdocsisBuf[10];

			DHCPdocsisBuf[11] = 0x00;
			DHCPpktcBuf[11] = 0x00;

		//	DHCPpktcBuf_offer[5] = DHCPpktcBuf[11];
		//	DHCPdocsisBuf_offer[5] = DHCPdocsisBuf[11];
		}
		else
		{
		//	DHCPpktcBuf_offer[5] = DHCPpktcBuf[11];
		//	DHCPdocsisBuf_offer[5] = DHCPdocsisBuf[11];
			
			DHCPdocsisBuf[11] += 0x01;
			DHCPpktcBuf[11] += 0x01;
		}
	}
	else
	{
		DHCPpktcBuf[10] = 0x00;
		DHCPdocsisBuf[10] = 0X00;
		DHCPpktcBuf[11] = 0x00;
		DHCPdocsisBuf[11] = 0X00;


	//	DHCPpktcBuf_offer[4] = 0x00;
	//	DHCPdocsisBuf_offer[4] = 0X00;
	//	DHCPpktcBuf_offer[5] = 0x00;
	//	DHCPdocsisBuf_offer[5] = 0X00;
		
	}

	//vlan tag
	//if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 2292)
	if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 2292 || ((DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktcBuf[15] & 0xff) < 2292)
	{
		if((DHCPpktcBuf[15] & 0xff) == 255 || (DHCPdocsisBuf[15] & 0xff) == 255)
		{	
			DHCPdocsisBuf[14] += 0x01;
			DHCPpktcBuf[14] += 0x01;

			DHCPdocsisBuf[15] = 0x00;
			DHCPpktcBuf[15] = 0x00;
		}
		else
		{
			DHCPpktcBuf[15] += 0x01;
			DHCPdocsisBuf[15] += 0X01;
		}
	}
	else
	{
		DHCPpktcBuf[14] = 0x08;
		DHCPdocsisBuf[14] = 0X08;
		DHCPpktcBuf[15] = 0x01;
		DHCPdocsisBuf[15] = 0X01;
	}
}


//void Option_Receive(int D_times, char sop, pcap_t *p_lan, pcap_t *p_wan)
void Option_Receive(int D_times, char sop)
{
	char errbuf[PCAP_ERRBUF_SIZE];

	/*LAN Port pcap send*/
	pcap_t *p_lan;
	
	//CASTLE USEING : ubuntu 12.04
	p_lan = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);
	
	if( p_lan == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	/*WAN port pacp send*/
	pcap_t *p_wan;

	p_wan = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);
	if( p_wan == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

		/*Set send time interval */
		struct timespec send_ts;
		send_ts.tv_sec = 1;
		send_ts.tv_nsec = 0;

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

				pthread_mutex_lock(&pcap_send_mutex);

				if(pcap_sendpacket(p_lan, DHCPdocsisBuf, sizeof(DHCPdocsisBuf)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return 1;
				}
				nanosleep(&send_ts,NULL);

				if(pcap_sendpacket(p_wan, DHCPdocsisBuf_offer, sizeof(DHCPdocsisBuf_offer)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return 1;
				}

				pthread_mutex_unlock(&pcap_send_mutex);
			}
			else if(Random_send >= 50 && Random_send <= 100)
			{
				DHCPBufMode = "pktc";

				pthread_mutex_lock(&pcap_send_mutex);

				if(pcap_sendpacket(p_lan, DHCPpktcBuf, sizeof(DHCPpktcBuf)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return 1;
				}

				nanosleep(&send_ts,NULL);

				if(pcap_sendpacket(p_wan, DHCPpktcBuf_offer, sizeof(DHCPpktcBuf_offer)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return 1;
				}

				pthread_mutex_unlock(&pcap_send_mutex);
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

		printf("**************************************************\n");
		printf("--------LAN ------------------------>  WAN--------\n");
		printf("Discover -> Compare Send packet and Receive packet\n");
		printf("False : %d\n",CompareFalseTimes);
		printf("True  : %d\n",CompareTrueTimes);
		printf("Discover -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n",D_times - (CompareFalseTimes + CompareTrueTimes));
		printf("\n");
		printf("--------LAN <------------------------  WAN--------\n");
		printf("OFFER -> Compare Send packet and Receive packet\n");
		printf("False : %d\n",CompareFalseTimes_offer);
		printf("True  : %d\n",CompareTrueTimes_offer);
		printf("OFFER -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n",D_times - (CompareFalseTimes_offer + CompareTrueTimes_offer));
		printf("***************************************************\n");
		printf("\n");

		/***** Init default value *****/
		CompareFalseTimes = 0;
		CompareTrueTimes = 0;
		CompareFalseTimes_offer = 0;
		CompareTrueTimes_offer = 0;
		LosePacket = 0;
		KeepRunning = 0;
		Running_Times = 0;
		AutoTesting = 0;
		StopLoopRunning = 0;
		D_times = 0;
		Menu("default");
		pcap_close(p_wan);
		pcap_close(p_lan);
		/******************************/
}


//compare word to enter
char buf;

void send_packet()
{
	//calculation send dhcp times
	int DHCPtimes = 0;

	/*char errbuf[PCAP_ERRBUF_SIZE];

	//LAN Port pcap send
	pcap_t *p_send;
	
	//CASTLE USEING : ubuntu 12.04
	p_send = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);
	
	if( p_send == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	//WAN port pacp send
	pcap_t *p_wan_send;

	p_wan_send = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);
*/
	//while((buf = getchar()) != NULL)
	while((buf = fgetc(stdin)) != NULL)
	{
		if(buf == '1' || buf == '2')
		{
	//		Option_Receive(DHCPtimes, buf, p_send, p_wan_send);
			Option_Receive(DHCPtimes, buf);
		}
		else if(buf == '3')
		{
			free(SendBuf);
			free(ReceiveBuf);
			free(SendpktcBuf);
			free(SendBuf_offer);
			free(ReceiveBuf_offer);
			free(SendpktcBuf_offer);
	//		pcap_close(p_send);
			exit(0);
		}
	}

	//pcap_close(p_wan_send);
	//pcap_close(p_send);
	
}


/*check process*/
void process_status()
{
	//test for check process status
	//signal(SIGALRM,SignalAlarmProcessStatus);

	int status;
	pid_t pid;

	if(!fork())
		return 1;

	pid = wait(&status);

	if(pid == -1)
		perror("wait");

	printf("pid = %d\n",pid);

	if(WIFEXITED(status))
		printf("Normal termination with exit status = %d\n", WEXITSTATUS(status));
	

	if(WIFSIGNALED(status))
		printf("killed by signal = %d%s\n", WTERMSIG(status), WCOREDUMP(status) ? "(dumped core)": "");

	if(WIFSTOPPED(status))
		printf("Stopped by signal = %d\n",WSTOPSIG(status));

	if(WIFCONTINUED(status))
		printf("Continued\n");
}



/*Send Packet thread*/
pthread_t pthreadSendPacket;

/*Receive Packet thread*/
pthread_t pthreadReadLoop;

/*Receive another lan port thread*/
pthread_t pthreadReadLoopLAN;

/*check process work status*/
pthread_t pthreadProcessStatus;

/*Thread socket*/
pthread_t pthreadSocketClient;

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
	SendBuf = (char*)malloc(sizeof(DHCPdocsisBuf));
	ReceiveBuf = (char*)malloc(sizeof(DHCPdocsisBuf));

	for(s = 18; s < sizeof(DHCPdocsisBuf); s++)
	{
		SendBuf[eth_ft++] = DHCPdocsisBuf[s];
	}

	int s_p = 0, eth_pk = 0;
	SendpktcBuf = (char*)malloc(sizeof(DHCPpktcBuf));
	for(s_p = 18; s_p < sizeof(DHCPpktcBuf); s_p++)
	{
		SendpktcBuf[eth_pk++] = DHCPpktcBuf[s_p];
	}
	/*******************************************/

	/*store send offer buf in char for compare********/
	int s_off = 0, eth_ftoff = 0;
	SendBuf_offer = (char*)malloc(sizeof(DHCPdocsisBuf_offer));
	ReceiveBuf_offer = (char*)malloc(sizeof(DHCPdocsisBuf_offer));

	for(s_off = 14; s_off < sizeof(DHCPdocsisBuf_offer); s_off++)
	{
		SendBuf_offer[eth_ftoff++] = DHCPdocsisBuf_offer[s_off];
	}

	int s_poff = 0, eth_pkoff = 0;
	SendpktcBuf_offer = (char*)malloc(sizeof(DHCPpktcBuf_offer));
	for(s_poff = 14; s_poff < sizeof(DHCPpktcBuf_offer); s_poff++)
	{
		SendpktcBuf_offer[eth_pkoff++] = DHCPpktcBuf_offer[s_poff];
	}
	/*******************************************/

	int pth_send = 0, pth_read = 0, pth_read_lan = 0;
	int pth_status = 0, pth_socket = 0;

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

	pth_read_lan = pthread_create(&pthreadReadLoopLAN, NULL, (void*)read_loop_lan, NULL);
	if( pth_read_lan != 0 )
	{
		printf("Create Read LAN Port Function Thread Error\n");
		printf("exit........................\n");
		exit(1);
	}

	/*Process status test*/
	/*pth_status = pthread_create(&pthreadProcessStatus, NULL, (void*)process_status, NULL);
	if( pth_status != 0 )
	{
		printf("Create Status Function Thread Error\n");
		printf("exit........................\n");
		exit(1);
	}*/

	/*Client Socket*/
	pth_socket = pthread_create(&pthreadSocketClient, NULL, (void*)ThreadClientSocket, NULL);
	if( pth_socket != 0 )
	{
		printf("Create Socket Function Thread Error\n");
		printf("exit........................\n");
		exit(1);
	}

	pthread_join(pthreadSendPacket, NULL);
	pthread_join(pthreadReadLoop, NULL);
	pthread_join(pthreadReadLoopLAN, NULL);
	//pthread_join(pthreadProcessStatus, NULL);
	pthread_join(pthreadSocketClient, NULL);

	free(SendBuf);
	free(SendpktcBuf);
	free(ReceiveBuf);
	free(SendBuf_offer);
	free(ReceiveBuf_offer);
	free(SendpktcBuf_offer);
	return 0;
}
