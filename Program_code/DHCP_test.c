#include "lib_file.h"
#include "IPHeader.h"

#define MAC_ADDRSTRLEN 2*6+5+1

char *SendBuf;
char *ReceiveBuf;

char *ChangeMode = "default";
char *DHCPBufMode = "default";

/*DHCP Discover buffer : docsis buffer*/
char DHCPdocsisBuf[] = {
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

/*DHCP Discover buffer : pktc buffer*/
char DHCPpktcBuf[] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x1c, 
0x7b, 0x22, 0x22, 0x23, 0x81, 0x00, 0x08, 0x01, 0x08, 0x00, 0x45, 0x00, 
0x02, 0x07, 0xde, 0xad, 0x00, 0x00, 0x80, 0x11, 
0x5a, 0x39, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 
0xff, 0xff, 0x00, 0x44, 0x00, 0x43, 0x01, 0xf3, 
0x81, 0xf6, 0x01, 0x01, 0x06, 0x00, 0x4f, 0x8a, 
0xe6, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 
0x7b, 0x22, 0x22, 0x23, 0x00, 0x00, 0x00, 0x00, 
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
0x02, 0x03, 0x06, 0x07, 0x0c, 0x0f, 0x36, 0x7a, 
0x3c, 0x7a, 0x70, 0x6b, 0x74, 0x63, 0x31, 0x2e, 
0x35, 0x3a, 0x30, 0x35, 0x33, 0x37, 0x30, 0x31, 
0x30, 0x31, 0x30, 0x31, 0x30, 0x32, 0x30, 0x31, 
0x30, 0x32, 0x30, 0x39, 0x30, 0x31, 0x30, 0x31, 
0x30, 0x62, 0x30, 0x35, 0x30, 0x36, 0x30, 0x39, 
0x30, 0x65, 0x30, 0x64, 0x30, 0x66, 0x30, 0x63, 
0x30, 0x31, 0x30, 0x31, 0x30, 0x64, 0x30, 0x31, 
0x30, 0x31, 0x30, 0x66, 0x30, 0x31, 0x30, 0x31, 
0x31, 0x30, 0x30, 0x31, 0x30, 0x39, 0x31, 0x32, 
0x30, 0x32, 0x30, 0x30, 0x30, 0x37, 0x31, 0x33, 
0x30, 0x31, 0x30, 0x31, 0x31, 0x34, 0x30, 0x31, 
0x30, 0x31, 0x31, 0x35, 0x30, 0x31, 0x30, 0x31, 
0x31, 0x36, 0x30, 0x31, 0x30, 0x31, 0x31, 0x37, 
0x30, 0x33, 0x30, 0x32, 0x30, 0x30, 0x33, 0x66, 
0x31, 0x38, 0x30, 0x31, 0x30, 0x30, 0x31, 0x39, 
0x30, 0x31, 0x30, 0x30, 0x2b, 0x59, 0x02, 0x04, 
0x45, 0x4d, 0x54, 0x41, 0x04, 0x06, 0x31, 0x31, 
0x31, 0x31, 0x31, 0x32, 0x05, 0x04, 0x56, 0x31, 
0x2e, 0x30, 0x06, 0x0d, 0x33, 0x2e, 0x39, 0x2e, 
0x32, 0x31, 0x2e, 0x31, 0x36, 0x6d, 0x70, 0x35, 
0x20, 0x07, 0x0a, 0x32, 0x2e, 0x34, 0x2e, 0x30, 
0x62, 0x65, 0x74, 0x61, 0x33, 0x08, 0x03, 0x00, 
0x1c, 0x7b, 0x09, 0x0b, 0x42, 0x43, 0x4d, 0x39, 
0x33, 0x33, 0x38, 0x33, 0x56, 0x43, 0x4d, 0x0a, 
0x08, 0x42, 0x72, 0x6f, 0x61, 0x64, 0x63, 0x6f, 
0x6d, 0x1f, 0x06, 0x00, 0x1c, 0x7b, 0x22, 0x22, 
0x23, 0x20, 0x04, 0xd3, 0x54, 0xb6, 0x5c, 0x3d, 
0x0f, 0xff, 0x7b, 0x22, 0x22, 0x23, 0x00, 0x03, 
0x00, 0x01, 0x00, 0x1c, 0x7b, 0x22, 0x22, 0x23, 
0x39, 0x02, 0x05, 0xdc, 0xff };

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
	
	printf("------------------------------- SVGM Mode ---------------------------\n");
	printf("-------------- LAN Port ----------- | ---------- WAN Port -----------\n");
	if(DHCPBufMode == "docsis")
	{
		printf("Destination 	: %17s | %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s | %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x            | 0x%04x\n", LAN_docsis_type, WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
		printf("         | %c%c%c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308],content[309],content[310]);

		printf("802.1Q Virtual LAN ID : %u        | %u\n",LAN_docsis_TPID,WAN_TPID);
	}
	else if(DHCPBufMode == "pktc")
	{
		printf("Destination 	: %17s | %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s | %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x            | 0x%04x\n",LAN_pktc_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		

		printf("           | %c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308]);

		printf("802.1Q Virtual LAN ID : %u        | %u\n",LAN_pktc_TPID,WAN_TPID);
		
		/*int eth_ft = 0;
		int s = 0;
	for(s = 18; s < sizeof(DHCPpktcBuf); s++)
	{
		SendBuf[eth_ft++] = DHCPpktcBuf[s];
	}*/
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

	
	printf("------------------------------- DVGM Mode ---------------------------\n");
	printf("-------------- LAN Port ----------- | ---------- WAN Port -----------\n");
	if(DHCPBufMode == "docsis")
	{
		printf("Destination 	: %17s | %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s | %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x            | 0x%04x\n",LAN_docsis_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
		printf("         | %c%c%c%c%c%c%c%c%c\n",content[298],content[299],content[300],content[301],content[302],content[303],content[304],content[305],content[306]);

		printf("802.1Q Virtual LAN ID : %u        | %u\n",LAN_docsis_TPID);
	}
	else if(DHCPBufMode == "pktc")
	{
	
		printf("Destination 	: %17s | %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s | %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x            | 0x%04x\n",LAN_pktc_type,WAN_type);
		
		printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		

		printf("           | %c%c%c%c%c%c%c\n", content[298], content[299], content[300], content[301], content[302], content[303], content[304]);

		printf("802.1Q Virtual LAN ID : %u        | %u\n", LAN_pktc_TPID);
	}
		
	/*Send buffer to ip structure*/
	dump_ip((ip_header*)(content + sizeof(eth_header) - 4));
}


void dump_ip(ip_header *ipv4)
{
		memset(ReceiveBuf,0,1024);
		char test1[] = {0x09,0x12};
		char *qq = test1;
		
		char test2[] = {0x09,0x12};
		char *tt = test2;
		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			ReceiveBuf = (char*)ipv4;
			SendBuf[0] += 9;
			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf("Compare data --------> %s\n", !strncmp(SendBuf, ReceiveBuf,sizeof(DHCPdocsisBuf)) ? "true" : "false");
		//	printf("Compare data --------> %s\n", !strncmp(test1,test2,sizeof(test1)) ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
		}

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


	int test = 1;

		inet_ntop(AF_INET, &ipv4 -> ip_src, src, sizeof(src));
		inet_ntop(AF_INET, &ipv4 -> ip_dst, dst, sizeof(dst));
	if(test == 0)
	{
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
	p_read = pcap_open_live("eth2", 65536, 1, 10, errbuf);
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

void send_packet()
{
	
	//calculation send dhcp times
	int DHCPtimes = 0;

	char errbuf[PCAP_ERRBUF_SIZE];

	pcap_t *p_send;
	
	//CASTLE USEING : ubuntu 12.04
	p_send = pcap_open_live("eth14", 65536, 1, 10, errbuf);
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
			ChangeMode = "DVGM";
			while(DHCPtimes < 50)
			{
				DHCPtimes++;
				printf("Send Times --------------> %d\n", DHCPtimes);
				DHCPBufMode = "docsis";
				if(pcap_sendpacket(p_send, DHCPdocsisBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
					return 1;
				}
				sleep(2);
				DHCPBufMode = "pktc";
				if(pcap_sendpacket(p_send, DHCPpktcBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
					return 1;
				}
				sleep(2);
				//cm mac 
				DHCPdocsisBuf[11] += 0x01;
				DHCPpktcBuf[11] += 0x01;
				//vlan tag
				DHCPdocsisBuf[15] += 0x01;
				DHCPpktcBuf[15] += 0x01;
				printf("\n");

				DHCPBufMode = "default";
			}
			DHCPtimes = 0;
		}
		else if(!(strcmp(buf, "SVGM\n\0")))
		{
			ChangeMode = "SVGM";
			while(DHCPtimes < 50)
			{
				DHCPtimes++;
				printf("Send Times --------------> %d\n", DHCPtimes);
				DHCPBufMode = "docsis";
			
				if(pcap_sendpacket(p_send, DHCPdocsisBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
					return 1;
				}
				sleep(2);

				DHCPBufMode = "pktc";
				if(pcap_sendpacket(p_send, DHCPpktcBuf, 1024) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
					return 1;
				}
				sleep(2);
				//cm mac 
				DHCPdocsisBuf[11] += 0x01;
				DHCPpktcBuf[11] += 0x01;
				//vlan tag
				DHCPdocsisBuf[15] += 0x01;
				DHCPpktcBuf[15] += 0x01;
				printf("\n");

				DHCPBufMode = "default";
			}
			DHCPtimes = 0;
		}

		/*else if(!(strcmp(buf, "pktc\n\0")))
		{
			printf("Sending PKTC buffer.............................\n");
			if(pcap_sendpacket(p_send, DHCPpktcBuf, 1024) < 0){
				fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_send));
				return 1;
			}
			printf("\n");
		}*/
		else if(!(strcmp(buf, "send\n\0")))
		{
			int showsendbuf = 0;
			int showrecebuf = 0;
			printf("**************Send buffer************************\n");
			for(; showsendbuf < sizeof(DHCPdocsisBuf); showsendbuf++)
			{
				printf("0x%02x,",SendBuf[showsendbuf] & 0xff);
			}
			printf("\n");
			printf("**************Receive buffer************************\n");
			for(; showrecebuf < sizeof(DHCPdocsisBuf); showrecebuf++)
			{
				printf("0x%02x,",ReceiveBuf[showrecebuf] & 0xff);
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
int main()
{
	int pth_send = 0;
	int pth_read = 0;

	int s = 0;
	int eth_ft = 0;

	SendBuf = malloc(1024);
	ReceiveBuf = malloc(1024);

	for(s = 18; s < sizeof(DHCPdocsisBuf); s++)
	{
		SendBuf[eth_ft++] = DHCPdocsisBuf[s];
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
