#include "lib_file.h"

#include <pcap.h>
#include <net/ethernet.h> 
#include <time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>


#define next_line printf("\n");
#define MAC_ADDRSTRLEN 2*6+5+1

/* 
struct pcap_pkthdr{
	struct timeval ts; //time stamp
	bpf_u_int32 caplen; // length of portion present
	bpf_u_int32 len; // length this packet(off wire)
};*/

void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header,const unsigned char *bytes)
{
	char timebuf[64];
	char addrstr[64];
	struct ether_header *ethhdr = (struct ether_header *)bytes;
//	struct iphdr *ipv4h;
	struct ip6_hdr *ipv6h;


	//set & get s_mac d_mac
	char dst_mac[MAC_ADDRSTRLEN] = {};
	char src_mac[MAC_ADDRSTRLEN] = {};

	//copy header
	strlcpy(dst_mac, mac_ntoa(ethhdr->ether_dhost),sizeof(dst_mac));
	strlcpy(src_mac, mac_ntoa(ethhdr->ether_shost),sizeof(dst_mac));

	//show receive mac address
	printf("Destination MAC address**********************\n");
	printf("%17s\n",dst_mac);
	printf("Source MAC address**************************\n");
	printf("%17s\n",src_mac);
	printf("//////////////////////////////////////////////\n");

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
		//	ipv4h = (struct iphdr*)(bytes + sizeof(struct ether_header));
		//	inet_ntop(AF_INET, &ipv4h->saddr, addrstr,sizeof(addrstr));
			printf("src[%s]\n",addrstr);

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

void dump_ip(u_int32_t length, const u_char *content)
{
	struct ip *ip = (struct ip*)(content + ETHER_HDR_LEN);
	u_char protocol = ip->ip_p;
	printf("Protocol : ip\n");


	switch(protocol){
		case IPPROTO_UDP:
			printf("UDP........\n");
		break;

		case IPPROTO_TCP:
			dump_tcp(length,content);
		break;

		case IPPROTO_ICMP:
			printf("ICMP............\n");
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

int main(int argc, char *argv[])
{
	pcap_t *p;
	char errbuf[PCAP_ERRBUF_SIZE];

	//open eth port
	p = pcap_open_live("eth14",65536,1,10,errbuf);
	if( p == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return 1;
	}

	if( pcap_loop(p,-1, pcap_handler_func,NULL) < 0 ){
		fprintf(stderr, "%s\n",pcap_geterr(p));
		pcap_close(p);
		return 1;
	}

	pcap_close(p);

	return 0;
}
