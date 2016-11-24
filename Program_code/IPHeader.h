#include "lib_file.h"

typedef struct S_ETH_HEADER
{
	//unsigned short xvid; 	// 2 Byte
	unsigned char dmac[6];  // 6 Byte
	unsigned char smac[6];  // 6 Byte
	unsigned short type;
	//#if d_vlan == 1
	unsigned short tpid; 	// 2 Byte
	unsigned short vid; 	// 2 Byte
	//#endif
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


typedef struct S_UDP_HEADER
{
	unsigned short udp_source;
	unsigned short udp_dest;
	unsigned short udp_len; // value : 8 -> this means udp header length
	unsigned short udp_check;
}__attribute__((__packed__)) udp_header;


typedef struct S_UDP_PACKET
{
	eth_header eth_frame;
	ip_header ip_frame;
	udp_header udp_frame;
}__attribute__((__packed__)) udp_pkt;

/*ARP hardware type*/
/*
	1 ---> Ethernet
	6 ---> Token Ring
   15 ---> Frame Relay
   16 ---> ATM
*/

/*ARP Operation Code*/
/*
	1 ---> ARP Request
	2 ---> ARP Reply
    3 ---> RARP Request
	4 ---> RARP Reply
*/

typedef struct S_ARP_PACKET
{
	unsigned short int arp_hrdtype; //arp hardware type
	unsigned short int arp_protocol; //arp protocol type
	unsigned char arp_hardln;   //length of hardware address normal value 6
	unsigned char arp_protoln;   //length of protocol address normal value 4
	unsigned short int arp_opcode; //arp operation code 
	unsigned char arp_shard[6]; //sender hardware address(mac)
	unsigned char arp_sip[4]; //sender ip
	unsigned char arp_thard[6]; //target hardware address(mac)
	unsigned char arp_tip[4]; //target ip
}__attribute__((__packed__)) arp_packet;

//typedef struct S_ICMP_PACKET
//{
	





//}


uint16_t ip_checksum(const void *buf, size_t hdr_len)
{
	unsigned long sum = 0;
	const uint16_t *ip1 = buf;

	while (hdr_len > 1)
	{
		sum += *ip1++;

		if (sum & 0x80000000)
			sum = (sum & 0xFFFF) + (sum >> 16);

		hdr_len -= 2;
	}

	while (sum >> 16){
		sum = (sum & 0xFFFF) + (sum >> 16);
	}

	return((unsigned short)~sum);
}


unsigned short ip_header_cksum(ip_header *ip, int len)
{
    long sum = 0;  /*  assume 32 bit long, 16 bit short */
	unsigned short * ptr = (unsigned short*)ip;

	while(len > 1){
		sum += *ptr;
			if(sum & 0x80000000)   /*  if high order bit set, fold */
				sum = (sum & 0xFFFF) + (sum >> 16);
		len -= 2;
		ptr++;
	}

	if(len)       /*  take care of left over byte */
		sum += (unsigned short) *(unsigned char *)ptr;

	while(sum>>16)
		sum = (sum & 0xFFFF) + (sum >> 16);
	return ~sum;
}

