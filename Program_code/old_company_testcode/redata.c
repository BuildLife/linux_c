#include "lib_file.h"
#include "IPHeader.h"

#define next_line printf("\n");
#define MAC_ADDRSTRLEN 2*6+5+1
//#define ETH_AND_VLAN 

/* 
struct pcap_pkthdr{
	struct timeval ts; //time stamp
	bpf_u_int32 caplen; // length of portion present
	bpf_u_int32 len; // length this packet(off wire)
};*/


unsigned char pkt1[] = {
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
//compare 
bool compare(unsigned char *buf1,unsigned char *buf2)
{
	int i = 0;
	for(i = 0;i<sizeof(eth2_buf);i++)
	{
		if(buf1[i] != buf2[i])
		return false;
	}
	return true;
	
}

//transformation mac address to string 
char *mac_ntoa(unsigned char *d)
{
	static char MacStr[MAC_ADDRSTRLEN];
	snprintf(MacStr,sizeof(MacStr), "%02x:%02x:%02x:%02x:%02x:%02x",d[0],d[1],d[2],d[3],d[4],d[5]);

	return MacStr;
}

char eth14_buf[1024];
char eth2_buf[1024];
void pcap_handler_func_t(unsigned char *user,const struct pcap_pkthdr *header,const unsigned char *bytes)
{
	printf("************************** Eth 2 Interface *********************************\n");
	printf("************************** Eth 2 Interface *********************************\n");
	printf("************************** Eth 2 Interface *********************************\n");
	printf("************************** Eth 2 Interface *********************************\n");
	char timebuf[64];
	char addrstr[64];
	char addrdes[64];


	int x = 0;
	int eth_tw = 0;
	for(x = 14;x<header->caplen;x++)
	{
		eth2_buf[eth_tw++] = bytes[x]; 
	}
	/*int t = 0;
	printf("eth2 buffer .........................................\n");
	for(t = 0;t<header->caplen;t++)
	{
		if(eth2_buf[t] != eth14_buf[t])
		{
			printf("NO : %d , 0x%02x is difference\n",t+1,eth2_buf[t]);
		}
	}*/

	printf("compare anser : %s\n",compare(eth2_buf,eth14_buf));
	//get ethernet type & value
	struct ether_header *ethhdr = (struct ether_header *)bytes;
	struct iphdr *ipv4h;
	struct ip6_hdr *ipv6h;


	//set & get s_mac d_mac
	char dst_mac[MAC_ADDRSTRLEN] = {};
	char src_mac[MAC_ADDRSTRLEN] = {};
	u_int16_t pack_type;
		
	//copy header
	strlcpy(dst_mac, mac_ntoa(ethhdr->ether_dhost),sizeof(dst_mac));
	strlcpy(src_mac, mac_ntoa(ethhdr->ether_shost),sizeof(dst_mac));
	pack_type = ntohs(ethhdr -> ether_type);

	if(pack_type <= 1500)
		printf("IEEE 802.3 Ethernet Frame:\n");
	else
		printf("Ethernet Frame:\n");

	//show receive mac address
	printf("Destination MAC address : %17s\n", dst_mac);
	printf("Source MAC address      : %17s\n", src_mac);
	next_line
	
	if (header->caplen < sizeof(struct iphdr) + sizeof(struct ether_header)){
		return;
	}

	memset(timebuf,0,sizeof(timebuf));

	//Convert Time to Character String => ctime_r(const time_t *time,char *buf);
	if(ctime_r(&header->ts.tv_sec, timebuf) == NULL)
	{
		return;
	}

	//delete the last '\0'<- space , change to next line.
	//timebuf[strlen(timebuf) - 1] = '\0'; 
	printf("%s",timebuf);
	printf("caplen:%d, len:%d, ",header->caplen,header->len);

	switch (ntohs(ethhdr->ether_type)){
		case ETHERTYPE_IP:
			printf("IPv4 : ");
			ipv4h = (struct iphdr*)(bytes + sizeof(struct ether_header));
			inet_ntop(AF_INET, &ipv4h->saddr, addrstr,sizeof(addrstr));
			inet_ntop(AF_INET, &ipv4h->daddr, addrdes,sizeof(addrdes));
			printf("src[%s]    des[%s]\n",addrstr,addrdes);
			

			//complite data packet
			dump_ip((struct ip*)(bytes + ETHER_HDR_LEN));
		break;

		case ETHERTYPE_IPV6:
			printf("IPv6 : ");
			ipv6h = (struct ip6_hdr *)(bytes + sizeof(struct ether_header));
			inet_ntop(AF_INET6, (void*)&ipv6h->ip6_src, addrstr, sizeof(addrstr));
			printf("src[%s]\n", addrstr);
		break;

		case ETHERTYPE_ARP:
			printf("arp.............\n");
			dump_arp(header->caplen, bytes);
		break;

		case ETH_P_8021Q:
			printf("vlan tag\n");
			dump_vlan_tag(header->caplen, bytes);
			break;

		default:
			printf("other\n");
		break;
	}

	next_line
	int i = 0;
	for(i = 0; i < header->caplen; i++)
	{
		printf("%02x,", bytes[i]);
	}
	next_line
	printf("**********************************************************\n");

	return;
}

void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header,const unsigned char *bytes)
{
	char timebuf[64];
	char addrstr[64];
	char addrdes[64];

	//get ethernet type & value
	struct ether_header *ethhdr = (struct ether_header *)bytes;
	struct iphdr *ipv4h;
	struct ip6_hdr *ipv6h;


	//set & get s_mac d_mac
	char dst_mac[MAC_ADDRSTRLEN] = {};
	char src_mac[MAC_ADDRSTRLEN] = {};
	u_int16_t pack_type;
		
	//copy header
	strlcpy(dst_mac, mac_ntoa(ethhdr->ether_dhost),sizeof(dst_mac));
	strlcpy(src_mac, mac_ntoa(ethhdr->ether_shost),sizeof(dst_mac));
	pack_type = ntohs(ethhdr -> ether_type);

	if(pack_type <= 1500)
		printf("IEEE 802.3 Ethernet Frame:\n");
	else
		printf("Ethernet Frame:\n");

	//show receive mac address
	printf("Destination MAC address : %17s\n", dst_mac);
	printf("Source MAC address      : %17s\n", src_mac);
	next_line
	
	if (header->caplen < sizeof(struct iphdr) + sizeof(struct ether_header)){
		return;
	}

	memset(timebuf,0,sizeof(timebuf));

	//Convert Time to Character String => ctime_r(const time_t *time,char *buf);
	if(ctime_r(&header->ts.tv_sec, timebuf) == NULL)
	{
		return;
	}

	//delete the last '\0'<- space , change to next line.
	//timebuf[strlen(timebuf) - 1] = '\0'; 
	printf("%s",timebuf);
	printf("caplen:%d, len:%d, ",header->caplen,header->len);

	switch (ntohs(ethhdr->ether_type)){
		case ETHERTYPE_IP:
			printf("IPv4 : ");
			ipv4h = (struct iphdr*)(bytes + sizeof(struct ether_header));
			inet_ntop(AF_INET, &ipv4h->saddr, addrstr,sizeof(addrstr));
			inet_ntop(AF_INET, &ipv4h->daddr, addrdes,sizeof(addrdes));
			printf("src[%s]    des[%s]\n",addrstr,addrdes);
			

			//complite data packet
			dump_ip((struct ip*)(bytes + ETHER_HDR_LEN));
		break;

		case ETHERTYPE_IPV6:
			printf("IPv6 : ");
			ipv6h = (struct ip6_hdr *)(bytes + sizeof(struct ether_header));
			inet_ntop(AF_INET6, (void*)&ipv6h->ip6_src, addrstr, sizeof(addrstr));
			printf("src[%s]\n", addrstr);
		break;

		case ETHERTYPE_ARP:
			printf("arp.............\n");
			dump_arp(header->caplen, bytes);
		break;

		case ETH_P_8021Q:
			printf("vlan tag\n");
			dump_vlan_tag(header->caplen, bytes);
			break;

		default:
			printf("other\n");
		break;
	}

	next_line
	int i = 0;
	for(i = 0; i < header->caplen; i++)
	{
		printf("%02x,", bytes[i]);
	}
	next_line
	printf("**********************************************************\n");

	return;
}

void dump_vlan_tag(u_int32_t length, const u_char *content)
{
	eth_header *ethhdr = (eth_header*)content;

	unsigned short tpid = htons(ethhdr->tpid);
	char src[64];
	char dst[64];
	printf("***************************************\n");
	printf("TPID --> %u\n",tpid);
	switch(ntohs(ethhdr -> vid))
	{	
		case ETHERTYPE_IP:
			{
				printf("UDP Type.......................................\n");
				ip_header *ipv4 = (ip_header*)(content + sizeof(eth_header));
				inet_ntop(AF_INET, &ipv4->ip_src, src, sizeof(src));
				inet_ntop(AF_INET, &ipv4->ip_dst, dst, sizeof(dst));
				printf("IP Source --> %s\n",src);
				printf("IP Destination --> %s\n",dst);
			}
			break;

		case ETHERTYPE_ARP:
			{
				printf("ARP Type..........................................\n");
				arp_packet *arp_p = (arp_packet*)(content + sizeof(eth_header));
				printf("ARP IP Source --> %u.%u.%u.%u\n",arp_p->arp_sip[0],arp_p->arp_sip[1],arp_p->arp_sip[2],arp_p->arp_sip[3]);
				printf("ARP IP Destination -->%u.%u.%u.%u\n",arp_p->arp_tip[0],arp_p->arp_tip[1],arp_p->arp_tip[2],arp_p->arp_tip[3]);

			}
			break;
		default:
			printf("others\n");
			break;

	}

	printf("****************************************\n");
}

void dump_arp(u_int32_t length, const u_char *content)
{
	static char *arp_ethtype[5] = {"NULL","Ethernet","Token Ring","Frame Relay","ATM"};
	static char *arp_code[5] = {"NULL","ARP_Request","ARP_Reply","RARP_Request","RARP_Reply"};

	arp_packet *arp = (arp_packet *)(content + ETHER_HDR_LEN);

	unsigned short int arp_type = htons(arp->arp_hrdtype);
	unsigned short int arp_protocol = htons(arp -> arp_protocol);
	int arp_hardsize = arp -> arp_hardln;
	int arp_protosize = arp -> arp_protoln;

	unsigned short int arp_opcode = htons(arp -> arp_opcode);
	
	unsigned char *arp_SEMac = arp -> arp_shard;
	unsigned char *arp_SEprotocol = arp -> arp_sip;
	unsigned char *arp_TAMac = arp -> arp_thard;
	unsigned char *arp_TAprotocol = arp -> arp_tip;


	printf("***********************************************\n");
	printf("ARP Hardware Type -> %s\n", arp_ethtype[arp_type]);
	printf("ARP Protocol -> 0x%04x  %-9s\n", arp_protocol, (arp_protocol == ETHERTYPE_IP) ? "IP" : "Not IP");
	printf("ARP hardware size -> %u\n", arp_hardsize);
	printf("ARP protocol size -> %u\n", arp_protosize);
	printf("ARP opcode -> %s\n", arp_code[arp_opcode]);
	printf("ARP Sender MAC -> %02x:%02x:%02x:%02x:%02x:%02x\n", arp_SEMac[0], arp_SEMac[1], arp_SEMac[2], arp_SEMac[3], arp_SEMac[4], arp_SEMac[5]);
    printf("ARP Sender protocol address -> %u.%u.%u.%u\n", arp_SEprotocol[0], arp_SEprotocol[1], arp_SEprotocol[2], arp_SEprotocol[3]);
	printf("ARP Target MAC -> %02x:%02x:%02x:%02x:%02x:%02x\n", arp_TAMac[0], arp_TAMac[1], arp_TAMac[2],arp_TAMac[3], arp_TAMac[4], arp_TAMac[5]);
	printf("ARP Target protocol address -> %u.%u.%u.%u\n", arp_TAprotocol[0], arp_TAprotocol[1], arp_TAprotocol[2], arp_TAprotocol[3]);
    printf("***********************************************\n");	
}

void dump_ip(struct ip* ip)
{
	u_char protocol = ip->ip_p;

	u_int8_t ip_tos = ip -> ip_tos; // type of service
	u_short ip_totallen =  ntohs(ip -> ip_len); //total length
	u_short ip_ident = ntohs(ip -> ip_id); //identification
	u_short ip_offset = ntohs(ip -> ip_off); //fragment offset field
	u_int8_t ip_ttl = ip -> ip_ttl;
	u_int8_t ip_pro = ip -> ip_p;
	u_short ip_checksum = ntohs(ip -> ip_sum);

	printf("***********************************************\n");
	printf("Type of service			-> %u\n", ip_tos);
	printf("Total length			-> %u\n", ip_totallen);
	printf("Identification			-> 0x%04x\n", ip_ident);
	printf("Fragment offset field	-> %u\n", ip_offset);
	printf("Time to live			-> %u\n", ip_ttl);
	printf("IP Protocol             -> %u\n", ip_pro);
	printf("IP Chceck sum			-> 0x%04x(%u)\n", ip_checksum,ip_checksum);
	printf("***********************************************\n");

	char *p = (char *)ip + (ip->ip_hl << 2);

	switch(protocol){
		case IPPROTO_TCP:
			printf("*********** UDP *************\n");
			dump_tcp((struct tcphdr*)p);
		break;

		case IPPROTO_UDP:
			printf("*********** TCP *************\n");
			dump_udp((struct udphdr*)p);
		break;

		case IPPROTO_ICMP:
			printf("*********** ICMP ************\n");
			dump_icmp((struct icmp*)p);
		break;

		case IPPROTO_IGMP:
			printf("*********** IGMP *************\n");
			dump_IGMP((struct igmp*)p);
		break;

		default:
			printf("Next is %d\n",protocol);
		break;
	}
}

void dump_tcp(struct tcphdr* tcp){

	u_int16_t source_port = ntohs(tcp->th_sport);
	u_int16_t destination_port = ntohs(tcp->th_dport);

	printf("Source Port : %5u\n", source_port);
	printf("Destination Port : %5u\n", destination_port);

}

void dump_udp(struct udphdr* udp)
{
	u_int16_t udp_source = ntohs(udp->uh_sport);
	u_int16_t udp_dest = ntohs(udp->uh_dport);
	u_int16_t udp_check = ntohs(udp->uh_sum);
	
	printf("***********************************************\n");
	printf("UDP Source -> %5u\n", udp_source);
	printf("UDP Destination ->%5u\n", udp_dest);
	printf("UDP checksum code ->%5x\n", udp_check);
	printf("***********************************************\n");
}


void dump_icmp(struct icmp *icmp)
{

	unsigned char type = icmp->icmp_type;
	unsigned char code = icmp->icmp_code;
	unsigned short checksum = htons(icmp->icmp_cksum);
	
	//identifier & sequence 
	unsigned short ident = icmp->icmp_id;
	unsigned short seq = icmp->icmp_seq;

	static char *ICMPtype[] = {
			"Echo Reply",
			"Undefined",
			"Undefined",
			"Distination",
			"Source Quench",
			"Redirect",
			"Undefined",
			"Undefined",
			"Echo Request",
			"Undefined",
			"Undefined",
			"Time Exeeded for a Datagram",
			"Parameter Problem on a Datagram",
			"Timestamp Request",
			"Timestamp Replay",
			"Information Request",
			"Information Reply",
			"Address Mask Request",
			"Address Mask Reply",
	};

	static char *ICMPcode[] = {
				"Network Unreachable",
				"Host Unreachable",
				"Protocol Unreachable",
				"Port Unreachable",
				"Fragmentation Needed and DF set",
				"Source Route Failed",
				"Destination network unknown",
				"Destination host unknown",
				"Source host isolated",
				"Communication with destination network administraively prohibited",
				"Communication with destination host administraively prohibited",
				"Network unreachable for type of service",
				"host unreachable for type of service",
				"Communication Administratively Prohibited",
				"Host precedence violation",
				"Precedence cutoff in effect",
	};

	printf("***********************************************\n");
	printf("ICMP Type -----> %u, %s\n", type, ICMPtype[type]);
	if(type == 3)
		printf("ICMP Code -----> %u, %s\n", code, ICMPcode[code]);
	else
	{
		printf("ICMP Code -----> %u\n", code);
		printf("Identifier ----> %u\n", ident);
		printf("Sequence ------> %u\n", seq);
	}
	printf("ICMP checksum code -> %5x\n", checksum);
	printf("***********************************************\n");



}

void dump_IGMP(struct igmp *igmp)
{
	u_int8_t igmp_type = igmp->igmp_type;
	u_int8_t igmp_code = igmp->igmp_code;
	u_int16_t igmp_checksum = igmp->igmp_cksum;
	
	
	printf("***********************************************\n");
	printf("IGMP type -> %5u\n", igmp_type);
	printf("IGMP code ->%5u\n", igmp_code);
	printf("IGMP checksum code ->%5x\n", igmp_checksum);
	printf("***********************************************\n");

}

unsigned char *get_mac(char *interface)
{
	int fd;
	struct ifreq ifr;
	unsigned char *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);

	mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;

	return mac;
}

unsigned char *SendUDPBuf()
{
	int len = 0;

	unsigned char *g_mac;
	g_mac = get_mac("eth14");

	unsigned char newbuf[128];
	
	eth_header *ethhdr = (eth_header*)newbuf;
	ip_header *ipv4 = (ip_header*)(newbuf + sizeof(eth_header));
	udp_header *udp_p = (udp_header*)(newbuf + sizeof(eth_header) + sizeof(ip_header));
	len = sizeof(eth_header) + sizeof(ip_header) + sizeof(udp_header);
	memset(newbuf, 0, sizeof(eth_header) + sizeof(ip_header) + sizeof(udp_header) + 16);
	
	//Destination mac address
	ethhdr->dmac[0] = 0xc4;
	ethhdr->dmac[1] = 0x12;
	ethhdr->dmac[2] = 0xf5;
	ethhdr->dmac[3] = 0x30;
	ethhdr->dmac[4] = 0x92;
	ethhdr->dmac[5] = 0xf9;

	//Source mac address
	ethhdr->smac[0] = g_mac[0];
	ethhdr->smac[1] = g_mac[1];
	ethhdr->smac[2] = g_mac[2];
	ethhdr->smac[3] = g_mac[3];
	ethhdr->smac[4] = g_mac[4];
	ethhdr->smac[5] = g_mac[5];

	ethhdr->type = htons(ETH_P_8021Q);
	ethhdr->tpid = htons(2049);
	
	ethhdr->vid = htons(0x0800);

	ipv4 -> ip_v = 5;
	ipv4 -> ip_hl = 4;
	ipv4 -> ip_tos = 0;
	ipv4 -> ip_len = htons(28);
	ipv4 -> ip_id = htons(0x1234);
	ipv4 -> ip_off = 0;
	ipv4 -> ip_ttl = 0xff;
	ipv4 -> ip_p = 17;
	ipv4 -> ip_sum = htons(0x1e3e);
	ipv4 -> ip_src = inet_addr("192.168.5.9");
	ipv4 -> ip_dst = inet_addr("192.168.5.5");

	udp_p -> udp_source = htons(1024);
	udp_p -> udp_dest = htons(1025);
	udp_p -> udp_len = htons(8);
	udp_p -> udp_check = htons(0x736b);
	
	return newbuf;
}

unsigned char *SendARPBuf()
{
	int len = 0;

	/*get mac*/
	unsigned char *g_mac;
	g_mac = get_mac("eth14");
		
	unsigned char sendbuf[128];

	eth_header *ethhdr = (eth_header*)sendbuf;
	arp_packet *arp_p = (arp_packet*)(sendbuf + sizeof(eth_header));
	len = sizeof(eth_header) + sizeof(arp_packet);
	memset(sendbuf, 0, sizeof(eth_header) + sizeof(ip_header) + 16);

	//Destination mac address
	ethhdr->dmac[0] = 0xc4;
	ethhdr->dmac[1] = 0x12;
	ethhdr->dmac[2] = 0xf5;
	ethhdr->dmac[3] = 0x30;
	ethhdr->dmac[4] = 0x92;
	ethhdr->dmac[5] = 0xf9;

	//Source mac address
	ethhdr->smac[0] = g_mac[0];
	ethhdr->smac[1] = g_mac[1];
	ethhdr->smac[2] = g_mac[2];
	ethhdr->smac[3] = g_mac[3];
	ethhdr->smac[4] = g_mac[4];
	ethhdr->smac[5] = g_mac[5];

	ethhdr->type = htons(ETH_P_8021Q);
	ethhdr->tpid = htons(2049);
	ethhdr->vid = htons(0x0806);


	unsigned char *arp_mac;
	arp_mac = get_mac("eth14");

	arp_p -> arp_hrdtype = htons(1);
	arp_p -> arp_protocol = htons(0x0800);
	arp_p -> arp_hardln = 6;
	arp_p -> arp_protoln = 4;
	arp_p -> arp_opcode = htons(2);

	arp_p -> arp_shard[0] = arp_mac[0];
	arp_p -> arp_shard[1] = arp_mac[1];
	arp_p -> arp_shard[2] = arp_mac[2];
	arp_p -> arp_shard[3] = arp_mac[3];
	arp_p -> arp_shard[4] = arp_mac[4];
	arp_p -> arp_shard[5] = arp_mac[5];

	arp_p -> arp_sip[0] = 192;
	arp_p -> arp_sip[1] = 168;
	arp_p -> arp_sip[2] = 5;
	arp_p -> arp_sip[3] = 5;

	
	arp_p -> arp_thard[0] = 0xc4;
	arp_p -> arp_thard[1] = 0x12;
	arp_p -> arp_thard[2] = 0xf5;
	arp_p -> arp_thard[3] = 0x30;
	arp_p -> arp_thard[4] = 0x92;
	arp_p -> arp_thard[5] = 0xf9;

	
	arp_p -> arp_tip[0] = 192;
	arp_p -> arp_tip[1] = 168;
	arp_p -> arp_tip[2] = 5;
	arp_p -> arp_tip[3] = 9;
	
	sendbuf[len++] = 0xfd;
	sendbuf[len++] = 0xde;
	sendbuf[len++] = 0x33;

	return sendbuf;
}
/*
unsigned char pkt1[] = {
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
0x12, 0x39, 0x02, 0x05, 0xdc, 0xff };*/
char *buf;
char *s_get;
int tx_len = 0;

void send_packet()
{
	/*get mac*/
	//unsigned char *g_mac;
	//g_mac = get_mac("eth14");

	pcap_t *p_send;

	buf = malloc(100*sizeof(char));
	s_get = malloc(100*sizeof(char));
	s_get = "send\n\0";
	char errbuf[PCAP_ERRBUF_SIZE];	

	p_send = pcap_open_live("eth14", 65536, 1, 10, errbuf);

	/*open eth port use in vmware ubuntu 16.04*/
	//pcap = pcap_open_live("ens33", 65536, 1, 10, errbuf);

	if( p_send == NULL ){
	fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
	 return 1;
	}
	int times = 0;
	while(fgets(buf, sizeof(buf), stdin) != NULL)
	{
		if(!(strcmp(buf, "udp\n\0")))
		{
			if(pcap_sendpacket(p_send, SendUDPBuf(), 256) < 0 ){
			fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
			}
		}
		else if(!(strcmp(buf, "arp\n\0")))
		{
			if(pcap_sendpacket(p_send, SendARPBuf(), 256) < 0 ){
			fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
			}
		}
		else if(!(strcmp(buf, "dhcp\n\0")))
		{
			while(times < 50 )
			{
				times++;
				pkt1[11] += 0x01;
				pkt1[15] += 0x01;
				if(pcap_sendpacket(p_send, pkt1, 1024) < 0 ){
				fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
				}
				sleep(1);
				printf("Times ====== %d\n",times);
			}
		}
		else if(!(strcmp(buf, "exit\n\0")))
		{
			exit(1);
			pcap_close(p_send);
		}
		else
			printf("error\n");
	}
	//fclose(fd);
	pcap_close(p_send);
}

void ploop()
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];	
	
	//open eth port
	//pcap = pcap_open_live("eth14", 65536, 1, 10, errbuf);
	//open eth port use in vmware ubuntu 16.04
	pcap = pcap_open_live("ens33", 65536, 1, 10, errbuf);

	if( pcap == NULL ){
	fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
	return 1;
	}
										
	if( pcap_loop(pcap,-1, pcap_handler_func,NULL) < 0 ){
	fprintf(stderr, "%s\n",pcap_geterr(pcap));
	pcap_close(pcap);
	return 1;
	}

	pcap_close(pcap);

}

void ploop_t()
{
	pcap_t *pcap_eth;
	char errbuf[PCAP_ERRBUF_SIZE];	
	
	struct bpf_program bpf_p;
	bpf_u_int32 net,mask;
	//open eth port
	//pcap = pcap_open_live("eth14", 65536, 1, 10, errbuf);
	//open eth port use in vmware ubuntu 16.04
	pcap_eth = pcap_open_live("eth2", 65536, 1, 10, errbuf);

	if( pcap_eth == NULL ){
	fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
	return 1;
	}
										
	if ( pcap_compile(pcap_eth, &bpf_p, "udp", 1, mask) == -1){
		fprintf(stderr, "pcap_compile: %s\n",pcap_geterr(pcap_eth));
		pcap_close(pcap_eth);
		exit(1);
	}

	if ( pcap_setfilter(pcap_eth, &bpf_p) == -1){
		fprintf(stderr, "pcap_setfilter: %s\n",pcap_geterr(pcap_eth));
		pcap_close(pcap_eth);
		exit(1);
	}

	pcap_freecode(&bpf_p);
	if( pcap_loop(pcap_eth,-1, pcap_handler_func_t,NULL) < 0 ){
	fprintf(stderr, "%s\n",pcap_geterr(pcap_eth));
	pcap_close(pcap_eth);
	return 1;
	}

	pcap_close(pcap_eth);
}

pthread_t pthreadSendPacket;
pthread_t pthreadPcapLoop;
pthread_t pthreadPcapLoop_t;

int main(int argc, char *argv[])
{

	//open eth port
/*	pcap = pcap_open_live("eth14", 65536, 1, 10, errbuf);
	//open eth port use in vmware ubuntu 16.04
	//pcap = pcap_open_live("ens33", 65536, 1, 10, errbuf);

	if( pcap == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}
*/
	/*if ( pcap_compile(pcap, &bpf_p, "tcp", 1, mask) == -1){
		fprintf(stderr, "pcap_compile: %s\n",pcap_geterr(pcap));
		pcap_close(pcap);
		exit(1);
	}*/

	/*if ( pcap_setfilter(pcap, &bpf_p) == -1){
		fprintf(stderr, "pcap_setfilter: %s\n",pcap_geterr(pcap));
		pcap_close(pcap);
		exit(1);
	}

	pcap_freecode(&bpf_p);*/

	/*int j = 0;
	int eth_ft = 0;
	printf("insert eth14 buffer...................................\n");
	for(j = 18; j < sizeof(pkt1);j++)
	{
		eth14_buf[eth_ft++] = pkt1[j];
		}
	int q = 0;
	printf("eth14 buffer....................................\n");
	for(q = 0;q < sizeof(pkt1);q++)
	{
		printf("0x%02x,",eth14_buf[q]);
	}
	next_line
	printf("////////////////////////////////////////////////////////////////////////////////");
	next_line*/
	
	int ret;
	int ret_l;
	int ret_lt;
	ret = pthread_create(&pthreadSendPacket,NULL,(void*)send_packet,NULL);
	if (ret != 0)
	{
		printf("create thread error\n");
		exit(1);
	}
	
	ret_l = pthread_create(&pthreadPcapLoop, NULL, (void*)ploop, NULL);
	if(ret_l != 0)
	{
		printf("create pcap loop thread error\n");
		exit(1);
	}

	/*ret_lt = pthread_create(&pthreadPcapLoop_t, NULL, (void*)ploop_t, NULL);
	if(ret_lt != 0)
	{
		printf("create pcap loop thread error\n");
		exit(1);
	}
*/
	pthread_join(pthreadPcapLoop,NULL);
	//pthread_join(pthreadPcapLoop_t,NULL);
	pthread_join(pthreadSendPacket,NULL);
/*	if( pcap_loop(pcap,-1, pcap_handler_func,NULL) < 0 ){
		fprintf(stderr, "%s\n",pcap_geterr(pcap));
		pcap_close(pcap);
		return 1;
	}

	pcap_close(pcap);
*/
	return 0;
}
