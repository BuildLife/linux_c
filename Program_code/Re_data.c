#include "lib_file.h"

#define next_line printf("\n");
#define MAC_ADDRSTRLEN 2*6+5+1

/* 
struct pcap_pkthdr{
	struct timeval ts; //time stamp
	bpf_u_int32 caplen; // length of portion present
	bpf_u_int32 len; // length this packet(off wire)
};*/

//transformation mac address to string 
char *mac_ntoa(unsigned char *d)
{
	static char MacStr[MAC_ADDRSTRLEN];
	snprintf(MacStr,sizeof(MacStr), "%02x:%02x:%02x:%02x:%02x:%02x",d[0],d[1],d[2],d[3],d[4],d[5]);

	return MacStr;
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
			dump_ip(header->caplen, bytes);
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

//struct  ether_arp {
//        struct  arphdr ea_hdr;          /* fixed-size header */
//        u_int8_t arp_sha[ETH_ALEN];     /* sender hardware address */
//        u_int8_t arp_spa[4];            /* sender protocol address */
 //       u_int8_t arp_tha[ETH_ALEN];     /* target hardware address */
 //       u_int8_t arp_tpa[4];            /* target protocol address */
//};

/*#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op*/

void dump_arp(u_int32_t length, const u_char *content)
{
	printf("Protocol : arp\n");
	struct ether_arp *arp = (struct ether_arp*)(content + ETHER_HDR_LEN);
	//u_int8_t *arp_SEhardware = arp->arp_sha;
	u_int8_t *arp_SEprotocol = arp->arp_spa;
	//u_int8_t *arp_TAhardware = arp->arp_tha;
	u_int8_t *arp_TAprotocol = arp->arp_tpa;

	int arp_hardsize = arp->ea_hdr.ar_hln;
	int arp_protosize = arp->ea_hdr.ar_pln;	

	printf("***********************************************\n");
        //printf("ARP Sender hareware address -> %5u.%5u.%5u.%5u\n", arp_SEhardware[0],arp_SEhardware[1],arp_SEhardware[2],arp_SEhardware[3]);
        printf("ARP Sender protocol address -> %u.%u.%u.%u\n", arp_SEprotocol[0],arp_SEprotocol[1],arp_SEprotocol[2],arp_SEprotocol[3]);
        //printf("ARP Target hareware address -> %5u.%5u.%5u.%5u\n", arp_TAhardware[0],arp_TAhardware[1],arp_TAhardware[2],arp_TAhardware[3]);
	printf("ARP Target protocol address -> %u.%u.%u.%u\n", arp_TAprotocol[0],arp_TAprotocol[1],arp_TAprotocol[2],arp_TAprotocol[3]);
	printf("ARP hardware size -> %u\n", arp_hardsize);
	printf("ARP protocol size -> %u\n", arp_protosize);
        printf("***********************************************\n");	
}

void dump_ip(u_int32_t length, const u_char *content)
{
	struct ip *ip = (struct ip*)(content + ETHER_HDR_LEN);
	u_char protocol = ip->ip_p;
	printf("Protocol : ip\n");


	switch(protocol){
		case IPPROTO_UDP:
			printf("*********** UDP *************\n");
			dump_udp(length,content);
		break;

		case IPPROTO_TCP:
			printf("*********** TCP *************\n");
			dump_tcp(length,content);
		break;

		case IPPROTO_ICMP:
			printf("*********** ICMP ************\n");
		break;

		case IPPROTO_IGMP:
			printf("*********** IGMP *************\n");
		break;

		default:
			printf("Next is %d\n",protocol);
		break;
	}
}

void dump_tcp(u_int32_t length, const u_char *content){

	struct ip *ip = (struct ip *)(content + ETHER_HDR_LEN);
	struct tcphdr *tcp = (struct tcphdr*)(content + sizeof(struct ether_header));

	u_int16_t source_port = ntohs(tcp->th_sport);
	u_int16_t destination_port = ntohs(tcp->th_dport);

	printf("Source Port : %5u\n", source_port);
	printf("Destination Port : %5u\n", destination_port);

}

void dump_udp(u_int32_t length, const u_char *content)
{
	struct ip *ip = (struct ip*)(content + ETHER_HDR_LEN);
	//struct udphdr *udp = (struct udphdr*)(content + sizeof(struct ether_header));
	struct udphdr *udp = (struct udphdr*)(content + ETHER_HDR_LEN + (ip->ip_hl << 2));
	u_int16_t udp_source = ntohs(udp->uh_sport);
	u_int16_t udp_dest = ntohs(udp->uh_dport);
	u_int16_t udp_check = ntohs(udp->uh_sum);
	
	printf("***********************************************\n");
	printf("UDP Source -> %5u\n", udp_source);
	printf("UDP Destination ->%5u\n", udp_dest);
	printf("UDP checksum code ->%5x\n", udp_check);
	printf("***********************************************\n");
}

void dump_IGMP(u_int32_t length,const u_char *content)
{
	struct ip *ip = (struct ip*)(content + ETHER_HDR_LEN);
	struct igmp *igmp = (struct igmp*)(content + ETHER_HDR_LEN + (ip -> ip_hl << 2));
	
	u_int8_t igmp_type = igmp->igmp_type;
	u_int8_t igmp_code = igmp->igmp_code;
	u_int16_t igmp_checksum = igmp->igmp_cksum;
	
	
	printf("***********************************************\n");
	printf("IGMP type -> %5u\n", igmp_type);
	printf("IGMP code ->%5u\n", igmp_code);
	printf("IGMP checksum code ->%5x\n", igmp_checksum);
	printf("***********************************************\n");

}

//void send_packet(pcap *p)
//{
	

//	if(pcap_sendpacket(pcap,send_packet, send_size) < 0 ){
//		fprintf(stderr, "pcap_sendpacket:%s\n",pcap_geterr(pcap));
//	}
//}


int main(int argc, char *argv[])
{
	pcap_t *pcap;
	char errbuf[PCAP_ERRBUF_SIZE];

	struct bpf_program bpf_p;
	bpf_u_int32 net,mask;

	//u_char send_packet[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x09";
	//int send_size = sizeof(send_packet) - 1;

	//open eth port
	//pcap = pcap_open_live("eth14", 65536, 1, 10, errbuf);
	//open eth port use in vmware ubuntu 16.04
	pcap = pcap_open_live("ens33", 65536, 1, 10, errbuf);

	if( pcap == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	/*if ( pcap_compile(pcap, &bpf_p, "arp", 1, mask) == -1){
		fprintf(stderr, "pcap_compile: %s\n",pcap_geterr(pcap));
		pcap_close(pcap);
		exit(1);
	}*/

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

	pcap_freecode(&bpf_p);
*/
//	if(pcap_sendpacket(pcap,send_packet, send_size) < 0 ){
//		fprintf(stderr, "pcap_sendpacket:%s\n",pcap_geterr(pcap));
//	}


	if( pcap_loop(pcap,-1, pcap_handler_func,NULL) < 0 ){
		fprintf(stderr, "%s\n",pcap_geterr(pcap));
		pcap_close(pcap);
		return 1;
	}

	pcap_close(pcap);

	return 0;
}
