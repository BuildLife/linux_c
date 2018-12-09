#include "lib_file.h"


/*Use to share memory function like malloc*/
void *xmalloc(size_t size)
{
	void *p;
	p = malloc(size);

	if(!p)
	{
		perror("xmalloc");
		exit(EXIT_FAILURE);
	}
	return p;
}


void InsertSendBuffer()
{
/*store send DHCP discover buf in char for compare********/
	int s_dis = 0, eth_ft = 0;
	SendBuf = xmalloc(sizeof(DHCPdocsisBuf));
	ReceiveBuf = xmalloc(sizeof(DHCPdocsisBuf));

	for(s_dis = 18; s_dis < sizeof(DHCPdocsisBuf); s_dis++)
	{
		SendBuf[eth_ft++] = DHCPdocsisBuf[s_dis];
	}

	int s_pkt = 0, eth_pk = 0;
	SendpktcBuf = xmalloc(sizeof(DHCPpktcBuf));
	for(s_pkt = 18; s_pkt < sizeof(DHCPpktcBuf); s_pkt++)
	{
		SendpktcBuf[eth_pk++] = DHCPpktcBuf[s_pkt];
	}
	/*************************************************/

	/*store send DHCP offer buf in char for compare********/
	int s_off = 0, eth_ftoff = 0;
	SendBuf_offer = xmalloc(sizeof(DHCPdocsisBuf_offer));
	ReceiveBuf_offer = xmalloc(sizeof(DHCPdocsisBuf_offer));

	for(s_off = 14; s_off < sizeof(DHCPdocsisBuf_offer); s_off++)
	{
		SendBuf_offer[eth_ftoff++] = DHCPdocsisBuf_offer[s_off];
	}

	int s_poff = 0, eth_pkoff = 0;
	SendpktcBuf_offer = xmalloc(sizeof(DHCPpktcBuf_offer));
	for(s_poff = 14; s_poff < sizeof(DHCPpktcBuf_offer); s_poff++)
	{
		SendpktcBuf_offer[eth_pkoff++] = DHCPpktcBuf_offer[s_poff];
	}
	/*************************************************/

	/*Strore send ARP buf in char for compare*********/
	int s_arp = 0, eth_arp = 0;
	SendBuf_arp = xmalloc(sizeof(ArpPacket));
	ReceiveBuf_arp = xmalloc(sizeof(ArpPacket));

	for(s_arp = 18; s_arp < sizeof(ArpPacket); s_arp++)
	{
		SendBuf_arp[eth_arp++] = ArpPacket[s_arp];
	}
	/*************************************************/
}



