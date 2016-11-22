#include "lib_file.h"
/*typedef struct S_VLAN_TYPE
{
	unsigned short vid

}*/

typedef struct S_ETH_HEADER
{
	//unsigned short xvid; 	// 2 Byte
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
	unsigned short ip_len;
	unsigned short ip_id; //identification
	unsigned short ip_off;
	unsigned char ip_ttl;
	unsigned char ip_p; //protocol
	unsigned short ip_sum;
	unsigned int ip_src;	//4 Byte
	unsigned int ip_dst;	//4 Byte
}__attribute__((__packed__)) ip_header;



typedef struct S_UDP_HEADER{
	unsigned short udp_source;
	unsigned short udp_dest;
	unsigned short udp_len;
	unsigned short udp_check;
}__attribute__((__packet__)) udp_header;


typedef struct S_UDP_PACKET{
	eth_header eth_frame;
	ip_header ip_frame;
	udp_header udp_frame;
}__attribute__((__packed__)) udp_pkt;

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

