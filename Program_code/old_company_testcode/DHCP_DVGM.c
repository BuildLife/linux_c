/*Basic lib*/
#define _BSD_SOURCE
#define _USE_BSD 
#include <stdio.h>
#include <stdlib.h>

/*use in true & false type : boolean type*/
#include <stdbool.h>

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

char *SendBuf;
char *ReceiveBuf;

/*Ethernet interface*/
char *Ether_in = "eth14";
char *Ether_out = "eth2";


/*DHCP Discover buffer : DVGM*/
char pkt1[] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1c, 
	0x7b, 0x11, 0x11, 0x12, 0x81, 0x00, 0x08, 0x01, 0x08, 0x00, 0x45, 0x00, 
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



//transformation mac address
char *mac_ntoa(unsigned char *mac_d)
{
	static char MACSTR[MAC_ADDRSTRLEN];
	snprintf(MACSTR, sizeof(MACSTR), "%02x:%02x:%02x:%02x:%02x:%02x", mac_d[0], mac_d[1], mac_d[2], mac_d[3], mac_d[4], mac_d[5]);

	return MACSTR;
}



void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{
	/*For take vlan tag*/
	eth_header *ethhdr = (eth_header*)bytes;


	char timebuf[64];
	memset(timebuf, 0, sizeof(timebuf));

	//set & get source mac and destination mac
	char dstmac[MAC_ADDRSTRLEN] = {};
	char srcmac[MAC_ADDRSTRLEN] = {};
	u_int16_t pack_type;

	//set mac
	strlcpy(dstmac, mac_ntoa(ethhdr -> dmac), sizeof(dstmac));
	strlcpy(srcmac, mac_ntoa(ethhdr -> smac), sizeof(srcmac));
	pack_type = ntohs(ethhdr -> type);
	
	//check buffer length have enough normal ethernet buffer
	if( header -> caplen < sizeof(ip_header) + sizeof(struct ether_header)){
		return;
	}

	if( ctime_r(&header -> ts.tv_sec, timebuf) == NULL)
	{
		return;
	}

	

	/*send buf vlan id*/
	unsigned short pkt_tpid = htons((pkt1[15] << 8) | pkt1[16]);

	/*receive buf checksum , must to mach send buf checksum*/
	unsigned short vlan_checksum = (bytes[18] << 8) | bytes[19];


	/*ethnet type*/
	unsigned short sendbuf_ethtype = pkt1[12] << 8 | pkt1[13];
	unsigned short receivebuf_ethtype = bytes[12] << 8 | bytes[13];

	if(vlan_checksum == 0xdead)
	{
		printf("Current Send Times : %s",timebuf);

		printf("-------------- LAN Port ----------- | ---------- WAN Port -----------\n");
	
		printf("Destination 	: %02x:%02x:%02x:%02x:%02x:%02x | %17s\n",pkt1[0]&0xff,pkt1[1]&0xff,pkt1[2]&0xff,pkt1[3]&0xff,pkt1[4]&0xff,pkt1[5]&0xff,dstmac);
	
		printf("Source      	: %02x:%02x:%02x:%02x:%02x:%02x | %17s\n",pkt1[6]&0xff,pkt1[7]&0xff,pkt1[8]&0xff,pkt1[9]&0xff,pkt1[10]&0xff,pkt1[11]&0xff,srcmac);
	
		printf("Ethernet Type 	: 0x%04x            | 0x%04x\n",sendbuf_ethtype, receivebuf_ethtype);
		
		printf("Option 60 class : %c%c%c%c%c%c%c%c%c",pkt1[302],pkt1[303],pkt1[304],pkt1[305],pkt1[306],pkt1[307],pkt1[308],pkt1[309],pkt1[310]);
		
		printf("         | %c%c%c%c%c%c%c%c%c\n",bytes[298],bytes[299],bytes[300],bytes[301],bytes[302],bytes[303],bytes[304],bytes[305],bytes[306]);

		printf("802.1Q Virtual LAN ID ---> %u\n",pkt_tpid);
		
		dump_ip((ip_header*)(bytes + sizeof(eth_header) - 4));
	}
}

void dump_ip(ip_header *ipv4)
{
		/*receive data*/
		int re = 0;
		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			ReceiveBuf = (char*)ipv4;
			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf("Compare data --------> %s\n", !strcmp(SendBuf, ReceiveBuf) ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
			memset(ReceiveBuf,0,1024);
		}
		/*
		char src[64];
		char dst[64];
		
		unsigned char ip_v = (((ipv4 -> ip_v)&0xff << 4) | (ipv4 -> ip_hl)&0xff);

		unsigned char iptos = ipv4 -> ip_tos;
		unsigned short iplen = htons(ipv4 -> ip_len);
		unsigned short ipid = htons(ipv4 -> ip_id);
		unsigned short ipoff = htons(ipv4 -> ip_off);

		unsigned char ipttl = ipv4 -> ip_ttl;
		unsigned char protocol = ipv4 -> ip_p;

		unsigned short ipchecksum = htons(ipv4 -> ip_sum);


		inet_ntop(AF_INET, &ipv4 -> ip_src, src, sizeof(src));
		inet_ntop(AF_INET, &ipv4 -> ip_dst, dst, sizeof(dst));
		
		printf("------------------ Internet Protocol Version 4 -----------------------\n");
		printf("IP Version   ------------------> %u\n", ip_v);
		printf("IP Type of Service   ----------> %u\n", iptos);
		printf("IP Length    ------------------> %d\n", iplen);
		printf("IP Identification   -----------> 0x%04x\n", ipid);
		printf("IP Fragment offset   ----------> %u\n", ipoff);
		printf("IP Time to Live   -------------> %u\n", ipttl);
		printf("IP Protocl   ------------------> %u\n", protocol);
		printf("IP Header Checksum  -----------> 0x%04x(%u)\n", ipchecksum, ipchecksum);
		printf("IP Source Address   -----------> %s\n", src);
		printf("IP Destination Address  -------> %s\n", dst);
		printf("***********************************************************************\n");
	
		udp_header *udp = (char*)ipv4 + (ipv4 -> ip_hl << 2) + 4;
		unsigned short udpSource = ntohs(udp -> udp_source);
		unsigned short udpdest = ntohs(udp -> udp_dest);
		unsigned short udplen = ntohs(udp -> udp_len);
		unsigned short udpchecksum = ntohs(udp -> udp_check);
		printf("------------------ User Datagram Protocol ---------------------------\n");
		printf("UDP Source Port ----------------> %5u\n", udpSource);
		printf("UDP Destination Port -----------> %5u\n", udpdest);
		printf("UDP Length ---------------------> %d\n", udplen);
		printf("UDP Checksum -------------------> 0x%04x\n", udpchecksum);
		printf("----------------------------------------------------------------------\n");
*/
}

/*Receive WAN Buffer*/
void read_loop()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net,mask;

	//CASTLE USEING : ubuntu 12.04
	p_read = pcap_open_live(Ether_out, 65536, 1, 10, errbuf);
	//ubuntu 16.04
	//p_read = pcap_open_live("ens33", 65536, 1, 10, errbuf);
	
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


//compare word to enter
char *buf;

/*Sending Buffer from LAN to WAN*/
void send_packet()
{
	
	//calculation send dhcp times
	int DHCPtimes = 0;

	char errbuf[PCAP_ERRBUF_SIZE];

	pcap_t *p_send;
	
	//CASTLE USEING : ubuntu 12.04
	p_send = pcap_open_live(Ether_in, 65536, 1, 10, errbuf);
	//ubuntu 16.04
	//p_send = pcap_open_live("ens33", 65536, 1, 10, errbuf);
	
	if( p_send == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	buf = malloc(100 * sizeof(char));
	while(fgets(buf, sizeof(buf), stdin) != NULL)
	{
		if(!(strcmp(buf, "DVGM\n\0")))
		{
			while(DHCPtimes < 50)
			{
				DHCPtimes++;
				printf("Send Times --------------> %d\n", DHCPtimes);
				if(pcap_sendpacket(p_send, pkt1, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
					return 1;
				}
				sleep(2);
				//cm mac 
				pkt1[11] += 0x01;
				//vlan tag
				pkt1[15] += 0x01;
				printf("\n");
			}
			DHCPtimes = 0;
		}
		else if(!(strcmp(buf, "send\n\0")))
		{
			int showbuf = 0;
			printf("\n");
			for(; showbuf < sizeof(pkt1); showbuf++)
			{
				printf("0x%02x,",SendBuf[showbuf] & 0xff);
			}
			printf("\n");

		}
		else if(!(strcmp(buf, "exit\n\0")))
		{
			free(SendBuf);
			free(ReceiveBuf);
			free(buf);
			pcap_close(p_send);
			exit(1);
		}
		else
			printf("Enter Error\n");
	}
	pcap_close(p_send);
	free(buf);
}


pthread_t pthreadSendPacket;
pthread_t pthreadReadLoop;



int main(int argc ,char*argv[])
{
	int pth_send = 0;
	int pth_read = 0;

	/*set Ethernet Interface*/
	Ether_in = argv[1];
	Ether_out = argv[2];

	printf("Ethernet Interface LAN ---> %s\n",argv[1]);
	printf("Ethernet Interface WAN ---> %s\n",argv[2]);


	int s = 0;
	int eth_ft = 0;
	SendBuf = malloc(1024);
	ReceiveBuf = malloc(1024);
	for(s = 18; s < sizeof(pkt1); s++)
	{
		SendBuf[eth_ft++] = pkt1[s];
	}
	
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
	free(ReceiveBuf);
	return 0;
}
