#include "lib_file.h"


/*Define Thread**************************/
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
/****************************************/

/*Define use function*/
void Signal_Stophandler();
void ThreadClientSocket();
void Memu(char *);
char *mac_ntoa(unsigned char *);
#if 0
void SVGM_Mode(u_int32_t , const u_int8_t *);
void DVGM_Mode(u_int32_t , const u_int8_t *);
#endif
void VGM_MODE(u_int32_t , const u_int8_t *);
void dump_DHCP_ip(ip_header *, int);
void dump_ARP_ip(arp_header *, int);
void dump_TFTP_ip(ip_header *, int);
void pcap_handler_func(unsigned char *, const struct pcap_pkthdr *, const unsigned char *);
void pcap_handler_func_lan(unsigned char *, const struct pcap_pkthdr *, const unsigned char *);
void read_loop();
void read_loop_lan();
void Option_Receive(int, char);
void send_packet();
void GetEthMACaddress(char []);


//Socket Client and Server Command
//Socket_Cmd socketcmd = {0}; 

//Send buffer
SendingBuffer SBr = {0};

/*Ethernet send port and receive port*/
char *LAN_port = "eth14";
char *WAN_port = "eth2";

/*Change DVGM or SVGM MODE*/
char *ChangeMode = "default";

/*Change Send docsis or pktc packet*/
char *DHCPBufMode = "default";

/*Set send times and VID Range*/
unsigned int Start_VID = 0;
unsigned int Running_Times = 0, KeepRunning = 0;

/*Record Compare data False times and True times*/
unsigned int CompareFalseTimes = 0, CompareTrueTimes = 0;
unsigned int CompareFalseTimes_offer = 0, CompareTrueTimes_offer = 0;
unsigned int CompareFalseTimes_arp = 0, CompareTrueTimes_arp = 0;

/*Socket Server for Auto testing flag 1 : open auto ; 0 : close auto*/
int AutoTesting = 0;

/*Record Lose packet*/
int Receivedocsispkt = 0, Receivepktcpkt = 0, Receivearppkt = 0;;

/*Pthread lock*/
pthread_mutex_t pcap_send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pcap_read_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Sending Packet for DHCP or ARP*/
char *PacketMode = "default";

int StopLoopRunning = 0;

/*Ctrl+c change the values for stop read/send while*/
void Signal_Stophandler()
{
	StopLoopRunning = 1;
}

/*Client socket for use*/
void ThreadClientSocket()
{	

	//Socket_Cmd->CLIENTOPEN = "CLIENTOPEN";
	//Socket_Cmd->SERVEROPEN = "SERVEROPEN";
	//Socket_Cmd->SERVERCLOSE = "CLOSE";
	int clientfd;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	int cl_addr = sizeof(struct sockaddr_in);
	char client_buffer[128];
	char sendserver_buffer[128];
	
	int GetTimesValue = 0;
	int GetStartVID = 0;
	fd_set rfds;
	int res = 0;

	//create socket
	/*udp socket*/
	clientfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(clientfd < 0)
	{
		perror("Open client socket Error\n");
	}
	else
	{
		printf("Open client socket success\n");
	}
	//Initialize client socket
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(9998);
	inet_pton(AF_INET, "127.0.0.1", &client_addr.sin_addr.s_addr);

	bzero(client_buffer, 128);
	bzero(sendserver_buffer, 128);

	char testmode = '0';

	//install CLIENTOPEN command for ready send to socket server 
	strcpy(sendserver_buffer, CLIENTOPEN);
	
	//tell socket server , client to connect
	sendto(clientfd, sendserver_buffer, 10, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

	sleep(1);
	recvfrom(clientfd, sendserver_buffer, sizeof(sendserver_buffer)-1, 0, (struct sockaddr *)&server_addr, &cl_addr);
		
	if(!strcmp(sendserver_buffer, SERVEROPEN))
	{
		printf("Connect to Server\n");
		
		//clean sendserver_buffer
		memset(sendserver_buffer, 0, sizeof(sendserver_buffer));
	}
		
	
	//Receive message from Server controller
	for(;;)
	{
		FD_ZERO(&rfds);
		FD_SET(clientfd, &rfds);
		if(select(clientfd+1, &rfds, NULL, NULL, NULL)) {
			if(FD_ISSET(clientfd, &rfds)) {
		/*udp socket*/
		res = recvfrom(clientfd, client_buffer, sizeof(client_buffer)-1, 0, (struct sockaddr *)&server_addr,&cl_addr);
		
			GetTimesValue = (client_buffer[1] & 0xff) << 8 | client_buffer[2] & 0xff;
			GetStartVID = (client_buffer[3] & 0xff) << 8 | client_buffer[4] & 0xff;
			
			if((client_buffer[0] != 0) && (client_buffer[5] == 0x01))
			{
				AutoTesting = 1;

				//set the loop running times
				Running_Times = GetTimesValue;

				//set start vid
				Start_VID = GetStartVID;
				if(Start_VID >= 2049 && Start_VID < 2512)
				{		
					DHCPdocsisBuf[14] = (Start_VID >> 8) & 0xff;
					DHCPdocsisBuf[15] = (Start_VID) & 0xff;
					DHCPpktcBuf[14] = (Start_VID >> 8) & 0xff;
					DHCPpktcBuf[15] = (Start_VID) & 0xff;
					
					/*Set ARP VLAN ID*/
					ArpPacket[14] = (Start_VID >> 8) & 0xff;
					ArpPacket[15] = (Start_VID) & 0xff;
				}
				
				if(Running_Times == 0)
				{
					/*Use in while loop always still Running*/
					KeepRunning = 1;

					/*It's for control stop while loop of Ctrl+c
					Avoid enter expect Ctrl+c, so in the start to set 0*/
					StopLoopRunning = 0;
				}

				if(client_buffer[0] == 1)
					testmode = '1';
				else if(client_buffer[0] == 2)
					testmode = '2';

				Option_Receive(0, testmode);

				client_buffer[0] = 0x53;
				client_buffer[1] = 0x54;
				client_buffer[2] = 0x4F;
				client_buffer[3] = 0x50;
				sendto(clientfd, client_buffer, 5, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

			}
			/*else if(client_buffer[6] == 0x43 && 
					client_buffer[7] == 0x4C && 
					client_buffer[8] == 0x4F && 
					client_buffer[9] == 0x53 && 
					client_buffer[10] == 0x45)
			{*/
			// It was still had problem need to fix.
			else if(strstr(client_buffer, SERVERCLOSE)){
					printf("Socket Server close\n")	;	
					pthread_cancel(pthreadSocketClient);
			}
			}
		}
	}
	close(clientfd);
	FD_CLR(clientfd, &rfds);
}


void Menu(char *mode)
{
	if(mode == "default")
	{
	    printf(" _______________ USER MENU _______________ \n");
		printf("|                                         |\n");
		printf("| 1.DVGM MODE                             |\n");
		printf("| 2.SVGM MODE                             |\n");
		printf("| 3.Exit this Program                     |\n");
		printf("| 4.Start Socket thread client            |\n");
		printf("| h.User Menu Show                        |\n");
		printf("| Ctrl+C --> Leave the sending loop       |\n");
		printf("|_________________________________________|\n");
		printf("\n");
		printf(":");
	}
	else if( mode == "DVGM" || mode == "SVGM" )
	{
		if(AutoTesting == 0)
		{
			printf("Enter Send Times or Enter 0 keep sending loop:\n");
			printf("Sending Times : ");
			scanf("%u", &Running_Times);
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
			scanf("%u", &Start_VID);
			if(Start_VID >= 2049 && Start_VID < 2512)
			{	
				/*Set DHCP VLAN ID*/
				DHCPdocsisBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPdocsisBuf[15] = (Start_VID) & 0xff;
				DHCPpktcBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPpktcBuf[15] = (Start_VID) & 0xff;
				
				/*Set ARP VLAN ID*/
				ArpPacket[14] = (Start_VID >> 8) & 0xff;
				ArpPacket[15] = (Start_VID) & 0xff;
			}
		}
	}
}

//transformation mac address
char *mac_ntoa(unsigned char *mac_d)
{
	static char MACSTR[MAC_ADDRSTRLEN];
	snprintf(MACSTR, sizeof(MACSTR), "%02x:%02x:%02x:%02x:%02x:%02x", mac_d[0], mac_d[1], mac_d[2], mac_d[3], mac_d[4], mac_d[5]);

	return MACSTR;
}
#if 0
/*DHCP SVGM Mode*/
void SVGM_Mode(u_int32_t length, const u_int8_t *content)
{
	/********************************* For LAN buffer **************************************/
	//LAN : set & get source mac and destination mac
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_arp_dstmac[MAC_ADDRSTRLEN] = {}, LAN_arp_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;
	u_int16_t LAN_arp_type;
	u_int16_t LAN_arp_TPID;

	//LAN : set mac 
	strlcpy(LAN_docsis_dstmac, mac_ntoa(LAN_docsis_ethhdr -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, mac_ntoa(LAN_docsis_ethhdr -> smac), sizeof(LAN_docsis_srcmac));
	strlcpy(LAN_pktc_dstmac, mac_ntoa(LAN_pktc_ethhdr -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, mac_ntoa(LAN_pktc_ethhdr -> smac), sizeof(LAN_pktc_srcmac));
	strlcpy(LAN_arp_dstmac, mac_ntoa(LAN_arp_ethhdr -> dmac), sizeof(LAN_arp_dstmac));
	strlcpy(LAN_arp_srcmac, mac_ntoa(LAN_arp_ethhdr -> smac), sizeof(LAN_arp_srcmac));

	//LAN : Ethernet type
	LAN_docsis_type = ntohs(LAN_docsis_ethhdr -> type);
	LAN_pktc_type = ntohs(LAN_pktc_ethhdr -> type);
	LAN_arp_type = ntohs(LAN_arp_ethhdr -> type);

	//LAN : TPID
	LAN_docsis_TPID = ntohs(LAN_docsis_ethhdr -> tpid);
	LAN_pktc_TPID = ntohs(LAN_pktc_ethhdr -> tpid);
	LAN_arp_TPID = ntohs(LAN_arp_ethhdr -> tpid);
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
	
	if(PacketMode == "DHCP") {
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
		dump_DHCP_ip((ip_header*)(content + sizeof(eth_header)),length - sizeof(eth_header));
	  }// if PacketMode == DHCP
	else if(PacketMode == "ARP")
	{
		printf("------------------------------- ARP Packet ---------------------------------------\n");
		printf("Destination 	: %17s        |   %17s\n",LAN_arp_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_arp_srcmac, WAN_srcmac);
		
		printf("Sender IP      	: %u.%u.%u.%u           |   %u.%u.%u.%u\n",ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, content[32]&0xff, content[33]&0xff, content[34]&0xff, content[35]&0xff);
		
		printf("Tender IP      	: %u.%u.%u.%u             |   %u.%u.%u.%u\n",ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, content[42]&0xff, content[43]&0xff, content[44]&0xff, content[45]&0xff);
		
		printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_arp_TPID,WAN_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));

	}//if PacketMode == ARP
}


/*DHCP DVGM Mode*/
void DVGM_Mode(u_int32_t length, const u_int8_t *content)
{
	/********************************* For LAN buffer **************************************/
	//LAN : set & get source mac and destination mac
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_arp_dstmac[MAC_ADDRSTRLEN] = {}, LAN_arp_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;
	u_int16_t LAN_arp_type;
	u_int16_t LAN_arp_TPID;

	//LAN : set mac 
	strlcpy(LAN_docsis_dstmac, mac_ntoa(LAN_docsis_ethhdr -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, mac_ntoa(LAN_docsis_ethhdr -> smac), sizeof(LAN_docsis_srcmac));
	strlcpy(LAN_pktc_dstmac, mac_ntoa(LAN_pktc_ethhdr -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, mac_ntoa(LAN_pktc_ethhdr -> smac), sizeof(LAN_pktc_srcmac));
	strlcpy(LAN_arp_dstmac, mac_ntoa(LAN_arp_ethhdr -> dmac), sizeof(LAN_arp_dstmac));
	strlcpy(LAN_arp_srcmac, mac_ntoa(LAN_arp_ethhdr -> smac), sizeof(LAN_arp_srcmac));

	//LAN : Ethernet type
	LAN_docsis_type = ntohs(LAN_docsis_ethhdr -> type);
	LAN_pktc_type = ntohs(LAN_pktc_ethhdr -> type);
	LAN_arp_type = ntohs(LAN_arp_ethhdr -> type);

	//LAN : TPID
	LAN_docsis_TPID = ntohs(LAN_docsis_ethhdr -> tpid);
	LAN_pktc_TPID = ntohs(LAN_pktc_ethhdr -> tpid);
	LAN_arp_TPID = ntohs(LAN_arp_ethhdr -> tpid);
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
	
	if(PacketMode == "DHCP") {
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
	  dump_DHCP_ip((ip_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
	}// if PacketMode == DHCP
	else if(PacketMode == "ARP")
	{
		printf("------------------------------- ARP Packet ---------------------------------------\n");
		
		printf("Destination 	: %17s        |   %17s\n",LAN_arp_dstmac, WAN_dstmac);
		
		printf("Source      	: %17s        |   %17s\n",LAN_arp_srcmac, WAN_srcmac);
		
		printf("Sender IP      	: %d.%d.%d.%d            |   %d.%d.%d.%d\n",ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, content[28]%0xff, content[29]%0xff, content[30]%0xff, content[31]%0xff);
		
		printf("Tender IP      	: %d.%d.%d.%d            |   %d.%d.%d.%d\n",ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, content[38]&0xff, content[39]&0xff, content[40]&0xff, content[41]&0xff);
		
		
		printf("802.1Q Virtual LAN ID : %u\n", LAN_arp_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
	}
}
#endif

//Combinded SVGM and DVGM in this Function
void VGM_MODE(u_int32_t length, const u_int8_t *content)
{
	/********************************* For LAN buffer **************************************/
	//docsis buffer 
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;

	strlcpy(LAN_docsis_dstmac, mac_ntoa(LAN_docsis_ethhdr -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, mac_ntoa(LAN_docsis_ethhdr -> smac), sizeof(LAN_docsis_srcmac));
		
	LAN_docsis_type = ntohs(LAN_docsis_ethhdr -> type);

	LAN_docsis_TPID = ntohs(LAN_docsis_ethhdr -> tpid);

	//pktc buffer
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};
	
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;

	strlcpy(LAN_pktc_dstmac, mac_ntoa(LAN_pktc_ethhdr -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, mac_ntoa(LAN_pktc_ethhdr -> smac), sizeof(LAN_pktc_srcmac));
	
	LAN_pktc_type = ntohs(LAN_pktc_ethhdr -> type);
	
	LAN_pktc_TPID = ntohs(LAN_pktc_ethhdr -> tpid);
	
	//arp buffer
	char LAN_arp_dstmac[MAC_ADDRSTRLEN] = {}, LAN_arp_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_arp_type;
	u_int16_t LAN_arp_TPID;

	strlcpy(LAN_arp_dstmac, mac_ntoa(LAN_arp_ethhdr -> dmac), sizeof(LAN_arp_dstmac));
	strlcpy(LAN_arp_srcmac, mac_ntoa(LAN_arp_ethhdr -> smac), sizeof(LAN_arp_srcmac));

	LAN_arp_type = ntohs(LAN_arp_ethhdr -> type);

	LAN_arp_TPID = ntohs(LAN_arp_ethhdr -> tpid);

	//tftp buffer
	/*char LAN_tftp_dstmac[MAC_ADDRSTRLEN] = {}, LAN_tftp_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_tftp_type;
	u_int16_t LAN_tftp_TPID;
	
	strlcpy(LAN_tftp_dstmac, mac_ntoa(LAN_tftp_ethhdr -> dmac), sizeof(LAN_tftp_dstmac));
	strlcpy(LAN_tftp_srcmac, mac_ntoa(LAN_tftp_ethhdr -> smac), sizeof(LAN_tftp_srcmac));

	LAN_tftp_type = ntohs(LAN_tftp_ethhdr -> type);

	LAN_tftp_TPID = ntohs(LAN_tftp_ethhdr -> tpid);
*/
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

	//WAN : TPID --> only for SVGM MODE
	WAN_TPID = ntohs(WAN_ethhdr -> tpid);
	/****************************************************************************************/

	printf("------------------------------- %s Mode ---------------------------------------\n", ChangeMode);
	printf("---------------- LAN Port ================ | ============> WAN Port -------------\n");
	printf("---------------- %s ---------------- | ------------ %s ------------\n", LAN_port, WAN_port);
	
	if(PacketMode == "DHCP")
	{
	  if(DHCPBufMode == "docsis")
	  {
		printf("Destination 	: %17s        |   %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_docsis_type,WAN_type);
		
	  	if(ChangeMode == "DVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
			printf("                |   %c%c%c%c%c%c%c%c%c\n",content[298],content[299],content[300],content[301],content[302],content[303],content[304],content[305],content[306]);

			printf("802.1Q Virtual LAN ID : %u\n",LAN_docsis_TPID);

			dump_DHCP_ip((ip_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
		}
		else if(ChangeMode == "SVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c%c%c",DHCPdocsisBuf[302],DHCPdocsisBuf[303],DHCPdocsisBuf[304],DHCPdocsisBuf[305],DHCPdocsisBuf[306],DHCPdocsisBuf[307],DHCPdocsisBuf[308],DHCPdocsisBuf[309],DHCPdocsisBuf[310]);
		
			printf("                |   %c%c%c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308],content[309],content[310]);

			printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_docsis_TPID,WAN_TPID);
			
			dump_DHCP_ip((ip_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));
		}
	  }//DHCPBufMode == "docsis"
	  else if(DHCPBufMode == "pktc")
	  {
	
		printf("Destination 	: %17s        |   %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_pktc_type,WAN_type);
		
	  	if(ChangeMode == "DVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		

			printf("                  |   %c%c%c%c%c%c%c\n", content[298], content[299], content[300], content[301], content[302], content[303], content[304]);
		
			printf("802.1Q Virtual LAN ID : %u\n", LAN_pktc_TPID);
	  	
			dump_DHCP_ip((ip_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
		}
		else if(ChangeMode == "SVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c",DHCPpktcBuf[302],DHCPpktcBuf[303],DHCPpktcBuf[304],DHCPpktcBuf[305],DHCPpktcBuf[306],DHCPpktcBuf[307],DHCPpktcBuf[308]);
		
			printf("                  |   %c%c%c%c%c%c%c\n",content[302],content[303],content[304],content[305],content[306],content[307],content[308]);

			printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_pktc_TPID,WAN_TPID);
	  	
			dump_DHCP_ip((ip_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));
		}
	  } //DHCPBufMode == "pktc" 
	}//PacketMode == "DHCP"
	else if(PacketMode == "ARP")
	{
		printf("------------------------------- ARP Packet ---------------------------------------\n");
		
		printf("Destination 	: %17s        |   %17s\n",LAN_arp_dstmac, WAN_dstmac);
		
		printf("Source      	: %17s        |   %17s\n",LAN_arp_srcmac, WAN_srcmac);
	  
	  if(ChangeMode == "DVGM")
	  {
		printf("Sender IP      	: %d.%d.%d.%d            |   %d.%d.%d.%d\n",ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, content[28]%0xff, content[29]%0xff, content[30]%0xff, content[31]%0xff);
		
		printf("Tender IP      	: %d.%d.%d.%d            |   %d.%d.%d.%d\n",ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, content[38]&0xff, content[39]&0xff, content[40]&0xff, content[41]&0xff);
		
		printf("802.1Q Virtual LAN ID : %u\n", LAN_arp_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
	  }
	  else if(ChangeMode == "SVGM")
	  {
		printf("Sender IP      	: %u.%u.%u.%u           |   %u.%u.%u.%u\n",ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, content[32]&0xff, content[33]&0xff, content[34]&0xff, content[35]&0xff);
		
		printf("Tender IP      	: %u.%u.%u.%u             |   %u.%u.%u.%u\n",ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, content[42]&0xff, content[43]&0xff, content[44]&0xff, content[45]&0xff);
		
		printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_arp_TPID,WAN_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));
	  }
	}//PacketMode == "ARP"
	/*else if(PacketMode == "TFTP")
	{
		
	  if(ChangeMode == "DVGM")
	  {
	  
	  
	  }
	  else if(ChangeMode == "SVGM")
	  {
	  
	  
	  }
	}//PacketMode == "TFTP"*/

}

void dump_DHCP_ip(ip_header *ipv4, int length)
{
		/*for discover buffer*/
		int compare_num = 0;
		int i = 0, x = 0;

		/*for offer buffer*/
		int compare_offer = 0;
		int i_offer = 0, x_offer = 0;

		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			//Record receive docsis packet
			Receivedocsispkt++;

			SBr.ReceiveBuf = (char*)ipv4;

			if(DHCPBufMode == "docsis")
			{
				for(i = 0; i < length; i++)
				{
					if(SBr.SendBuf[i] != SBr.ReceiveBuf[i])
					{
						compare_num += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(x = 0; x < length; x++)
				{
					if(SBr.SendpktcBuf[x] != SBr.ReceiveBuf[x])
					{
						compare_num += 1;
					}
				}
			}
			
			if(compare_num > 0)
				CompareFalseTimes += 1;
			else
				CompareTrueTimes += 1;

			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf(" '%s' Compare data --------> %s\n", DHCPBufMode, !compare_num ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
			memset(SBr.ReceiveBuf, 0, length);
			
			/*for test*/
			if((!compare_num) == 0)
				StopLoopRunning = 1;

		}//htons(ipv4 -> ip_id)
		else if(htons(ipv4 -> ip_sum) == 0xa381 || htons(ipv4 -> ip_sum) == 0xa3ef)
		{
			//Record receive pktc packet
			Receivepktcpkt++;

			SBr.ReceiveBuf_offer = (char*)ipv4;
			if(DHCPBufMode == "docsis")
			{
				for(i_offer = 0; i_offer < length; i_offer++)
				{
					if(SBr.SendBuf_offer[i_offer] != SBr.ReceiveBuf_offer[i_offer])
					{
						compare_offer += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(x_offer = 0; x_offer < length; x_offer++)
				{
					if(SBr.SendpktcBuf_offer[x_offer] != SBr.ReceiveBuf_offer[x_offer])
					{
						compare_offer += 1;
					}
				}
			}
			
			if(compare_offer > 0)
				CompareFalseTimes_offer += 1;
			else
				CompareTrueTimes_offer += 1;

			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf(" '%s offer' Compare data --------> %s\n", DHCPBufMode,!compare_offer ? "true" : "false");
			printf("*****************************************************************\n");
			
			/*Clear ReceiveBuf to zero*/
			memset(SBr.ReceiveBuf_offer, 0, length);
			/*for test*/
			/*if((!compare_offer) == 0)
			{
				StopLoopRunning = 1;
			}*/
		}//htons(ipv4 -> ip_sum)

}


void dump_ARP_ip(arp_header *arp_ipv4, int length)
{
	//Change receive buf that sender ip
	char receive_sender_ip[16] = {0};
	inet_ntop(AF_INET, &arp_ipv4 -> arp_sender_ip, receive_sender_ip, sizeof(receive_sender_ip));
	

	//Change send buf that sender ip
	char send_sender_ip[16] = {0};
	sprintf(send_sender_ip, "%d.%d.%d.%d", ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff);
	
	int compare_arp = 0;
	int i = 0, j = 0;

	//According to receive buf and send buf all have sender ip to compare.
	//Avoid the others data mix in it.
	if(!strcmp(send_sender_ip, receive_sender_ip)) {
		//Record Receive packet 
		Receivearppkt++;

		SBr.ReceiveBuf_arp = (char*)arp_ipv4;
		
		for(i = 0; i < length; i++)
		{
			if(SBr.SendBuf_arp[i] != SBr.ReceiveBuf_arp[i])
				compare_arp += 1;
		}

		printf("\n");
		printf("***************** ARP Compare Data **********************************\n");
		printf(" 'ARP' Compare data --------> %s\n", !compare_arp ? "true" : "false");
		printf("*****************************************************************\n");
				
		if(compare_arp > 0)
			CompareFalseTimes_arp += 1;
		else
			CompareTrueTimes_arp += 1;

	  	memset(SBr.ReceiveBuf_arp, 0, length);
	  }
}

void dump_TFTP_ip(ip_header *ipv4, int length)
{


}



/*WAN Port read Function*/
void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{

	//check buffer length is enough normal ethernet buffer
	if( header -> caplen < sizeof(ip_header) + sizeof(struct ether_header))
	{
		return;
	}

	char timebuf[64];
	memset(timebuf, 0, sizeof(timebuf));
	if( ctime_r(&header -> ts.tv_sec, timebuf) == NULL)
	{
		return;
	}
	

	char send_sender_ip[16] = {0};
	char receive_sender_ip[16] = {0};
	sprintf(send_sender_ip, "%d.%d.%d.%d", ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff);

	//identifi receive was the same buffer
	unsigned short BufferIdentification;

	if(ChangeMode == "DVGM")
	{
		BufferIdentification = (bytes[18] << 8) | bytes[19];
		sprintf(receive_sender_ip, "%d.%d.%d.%d", bytes[28]&0xff, bytes[29]&0xff, bytes[30]&0xff, bytes[31]&0xff);
	}
	else if(ChangeMode == "SVGM")
	{
		BufferIdentification = (bytes[22] << 8) | bytes[23];
		sprintf(receive_sender_ip, "%d.%d.%d.%d", bytes[32]&0xff, bytes[33]&0xff, bytes[34]&0xff, bytes[35]&0xff);
	}

	if(BufferIdentification == 0xdead && PacketMode == "DHCP")
	{
		printf("Current Send Times : %s", timebuf);
		VGM_MODE(header -> caplen, bytes);
	}
	else if(PacketMode == "ARP" && !strcmp(send_sender_ip, receive_sender_ip))
	{
		printf("Current Send Times : %s", timebuf);
		VGM_MODE(header -> caplen, bytes);
	}
	else if(PacketMode == "TFTP")
	{
		if(BufferIdentification == 0x0006 || BufferIdentification == 0x0017)
		{
			printf("Current Send Times : %s", timebuf);
			VGM_MODE(header -> caplen, bytes);
		}
	}
#if 0
	/*Send buffer to ip structure*/
	if(ChangeMode == "DVGM")
	{
		unsigned short DVGM_checksum = (bytes[18] << 8) | bytes[19];
		sprintf(receive_sender_ip, "%d.%d.%d.%d", bytes[28]&0xff, bytes[29]&0xff, bytes[30]&0xff, bytes[31]&0xff);
		
		if(DVGM_checksum == 0xdead && PacketMode == "DHCP")
		{
			printf("Current Send Times : %s", timebuf);
			DVGM_Mode(header -> caplen, bytes);
		}
		else if(PacketMode == "ARP" && !strcmp(send_sender_ip, receive_sender_ip))
		{
			printf("Current Send Times : %s", timebuf);
			DVGM_Mode(header -> caplen, bytes);
		}
		else if(PacketMode == "TFTP")
		{
			if(DVGM_checksum == 0x0006 || DVGM_checksum == 0x0017)
			{
				printf("Current Send Times : %s", timebuf);
				DVGM_Mode(header -> caplen, bytes);
			}
		}
	}
	else if(ChangeMode == "SVGM")
	{
		unsigned short SVGM_checksum = (bytes[22] << 8) | bytes[23];
		sprintf(receive_sender_ip, "%d.%d.%d.%d", bytes[32]&0xff, bytes[33]&0xff, bytes[34]&0xff, bytes[35]&0xff);
		
		if(SVGM_checksum == 0xdead && PacketMode == "DHCP")
		{
			printf("Current Send Times : %s", timebuf);
			SVGM_Mode(header -> caplen, bytes);
		}
		else if(PacketMode == "ARP" && !strcmp(send_sender_ip, receive_sender_ip))
		{
			printf("Current Send Times : %s", timebuf);
			SVGM_Mode(header -> caplen, bytes);
		}
		else if(PacketMode == "TFTP")
		{
			if(SVGM_checksum == 0x0006 || SVGM_checksum == 0x0017)
			{
				printf("Current Send Times : %s", timebuf);
				DVGM_Mode(header -> caplen, bytes);
			}
		}
	}
	#endif
}


/*LAN Port read Function*/
void pcap_handler_func_lan(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{
	unsigned short checksum_offer = (bytes[24] << 8) | bytes[25];

	//check buffer length have enough normal ethernet buffer
	if( header -> caplen < sizeof(ip_header) + sizeof(struct ether_header))
	{
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
			printf("Source IP       : %s             |   %u.%u.%u.%u\n",docsis_src, DHCPdocsisBuf_offer[26] & 0xff, DHCPdocsisBuf_offer[27] & 0xff, DHCPdocsisBuf_offer[28] & 0xff, DHCPdocsisBuf_offer[29] & 0xff);
			printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",WAN_docsis_type, LAN_type);
		
		}
		else if(DHCPBufMode == "pktc")
		{
			printf("------------------------------- PKTC OFFER --------------------------------------\n");
			printf("Destination 	: %17s        |   %17s\n",WAN_pktc_dstmac_offer, LAN_dstmac_offer);
	
			printf("Source      	: %17s        |   %17s\n",WAN_pktc_srcmac_offer, LAN_srcmac_offer);
			printf("Destination IP  : %s           |   %u.%u.%u.%u\n",pktc_dst, DHCPpktcBuf_offer[30] & 0xff, DHCPpktcBuf_offer[31] & 0xff, DHCPpktcBuf_offer[32] & 0xff, DHCPpktcBuf_offer[33] & 0xff);
			printf("Source IP       : %s             |   %u.%u.%u.%u\n",pktc_src, DHCPpktcBuf_offer[26] & 0xff, DHCPpktcBuf_offer[27] & 0xff, DHCPpktcBuf_offer[28] & 0xff, DHCPpktcBuf_offer[29] & 0xff);
	
			printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",WAN_pktc_type, LAN_type);
		
		}

		dump_DHCP_ip((ip_header*)(bytes + sizeof(eth_header) - 4), header -> caplen - sizeof(eth_header) - 4);
	}	
}

/*Use to filter UDP or ARP buffer and keep reading WAN port buffer*/
void read_loop()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net, mask;

	char protocol_filter[16];


	if(PacketMode == "DHCP" || PacketMode == "TFTP")
		sprintf(protocol_filter, "%s", "udp");
	else if(PacketMode == "ARP")
		sprintf(protocol_filter, "%s", "arp");

	//CASTLE USEING : ubuntu 12.04
	p_read = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);

	if( p_read == NULL ){
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return;
	}

	if( pcap_compile(p_read, &bpf_p, protocol_filter, 1, mask) == -1){
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


/*Use to filter udp buffer and keep reading LAN port buffer*/
void read_loop_lan()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net, mask;

	//read lan port
	pcap_t *p_read_lan;
	p_read_lan = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);

	if( p_read_lan == NULL )
	{
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return;
	}

	if( pcap_compile(p_read_lan, &bpf_p, "udp", 1, mask) == -1)
	{
		fprintf(stderr, "pcap_comiple:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	if( pcap_setfilter(p_read_lan, &bpf_p) == -1)
	{
		fprintf(stderr, "pcap_setfilter:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	pcap_freecode(&bpf_p);

	if (pcap_loop(p_read_lan, -1, pcap_handler_func_lan, NULL) < 0)
	{
		fprintf(stderr, "pcap_read:%s\n", pcap_geterr(p_read_lan));
		pcap_close(p_read_lan);
		exit(1);
	}

	pcap_close(p_read_lan);
}


void Option_Receive(int D_times, char sop)
{
	/*Random Sending data*/
	int Random_send = 0;

	char errbuf[PCAP_ERRBUF_SIZE];

	/*LAN Port pcap send*/
	pcap_t *p_lan;
	
	//CASTLE USEING : ubuntu 12.04
	p_lan = pcap_open_live(LAN_port, 65536, 1, 10, errbuf);
	
	if( p_lan == NULL )
	{
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return;
	}

	/*WAN port pacp send*/
	pcap_t *p_wan;

	p_wan = pcap_open_live(WAN_port, 65536, 1, 10, errbuf);
	if( p_wan == NULL )
	{
		fprintf(stderr, "Couldn't find default device : %s\n", errbuf);
		return;
	}

		/*Set send time interval */
		struct timespec send_ts;
		send_ts.tv_sec = 0;
		send_ts.tv_nsec = 500000000;

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
					
					SBr.SendBuf_arp[11] = 0x11;
					ArpPacket[9] = 0x11;
					ArpPacket[29] = 0x11;

					if(pcap_sendpacket(p_lan, DHCPdocsisBuf, sizeof(DHCPdocsisBuf)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "DHCP";

					nanosleep(&send_ts, NULL);

					//Sending ARP Packet
					if(pcap_sendpacket(p_lan, ArpPacket, sizeof(ArpPacket)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "ARP";

					nanosleep(&send_ts, NULL);

					//Sending TFTP Packet
					/*if(pcap_sendpacket(p_lan, tftpPacket_docsis, sizeof(tftpPacket_docsis)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "TFTP";

					nanosleep(&send_ts, NULL);
*/
					if(pcap_sendpacket(p_wan, DHCPdocsisBuf_offer, sizeof(DHCPdocsisBuf_offer)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}

					pthread_mutex_unlock(&pcap_send_mutex);
			}
			else if(Random_send >= 50 && Random_send <= 100)
			{
					DHCPBufMode = "pktc";

					pthread_mutex_lock(&pcap_send_mutex);
					
					SBr.SendBuf_arp[11] = 0x22;
					ArpPacket[9] = 0x22;
					ArpPacket[29] = 0x22;


					if(pcap_sendpacket(p_lan, DHCPpktcBuf, sizeof(DHCPpktcBuf)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "DHCP";

					nanosleep(&send_ts, NULL);
			
					//Sending ARP Packet
					if(pcap_sendpacket(p_lan, ArpPacket, sizeof(ArpPacket)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "ARP";

					nanosleep(&send_ts, NULL);

					//Sending TFTP Packet
/*					if(pcap_sendpacket(p_lan, tftpPacket_emta, sizeof(tftpPacket_emta)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
					}
					else
						PacketMode = "TFTP";

					nanosleep(&send_ts, NULL);
*/
					if(pcap_sendpacket(p_wan, DHCPpktcBuf_offer, sizeof(DHCPpktcBuf_offer)) < 0){
						fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
						pthread_mutex_unlock(&pcap_send_mutex);
						return;
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
		printf("--------LAN  ==> ==> ==> ==> ==> ==>   WAN--------\n");
		printf("Discover -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes);
		printf("True  : %d\n", CompareTrueTimes);
		printf("Discover -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", D_times - Receivedocsispkt);
		printf("..................................................\n");
		printf("ARP -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes_arp);
		printf("True  : %d\n", CompareTrueTimes_arp);
		printf("ARP -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", D_times - Receivearppkt);
		printf("\n");
		printf("--------LAN  <== <== <== <== <== <==   WAN--------\n");
		printf("OFFER -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes_offer);
		printf("True  : %d\n", CompareTrueTimes_offer);
		printf("OFFER -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", D_times - Receivepktcpkt);
		printf("***************************************************\n");
		printf("\n");

		/***** Init default value *****/
		CompareFalseTimes = 0;
		CompareTrueTimes = 0;
		CompareFalseTimes_offer = 0;
		CompareTrueTimes_offer = 0;
		CompareTrueTimes_arp = 0;
		CompareFalseTimes_arp = 0;

		//Init Lose packet record count
		Receivedocsispkt = 0;
		Receivepktcpkt = 0;
		Receivearppkt = 0;

		KeepRunning = 0;
		Running_Times = 0;
		AutoTesting = 0;
		StopLoopRunning = 0;
		D_times = 0;
		Menu("default");
		/******************************/
			
		pcap_close(p_wan);
		pcap_close(p_lan);
}


void send_packet()
{
	//option
	char Cmdbuf;

	//calculation send dhcp times
	int DHCPtimes = 0;

	//use in socket client
	int pth_socket = 0;

	while((Cmdbuf = fgetc(stdin)) != EOF)
	{
		if(Cmdbuf == '1' || Cmdbuf == '2')
		{
			Option_Receive(DHCPtimes, Cmdbuf);
		}
		else if(Cmdbuf == '3')
		{
			exit(0);
		}
		else if(Cmdbuf == '4')
		{
			/*Client Socket*/
			pth_socket = pthread_create(&pthreadSocketClient, NULL, (void*)ThreadClientSocket, NULL);
			if( pth_socket != 0 )
			{
				printf("Create Socket Function Thread Error\n");
				printf("exit........................\n");
				exit(1);
			}
			send_packet();
			pthread_join(pthreadSocketClient, NULL);
		}
		else if(Cmdbuf == 'h' || Cmdbuf == 'H')
		{
			Menu("default");
		}
	}
}

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

	GetEthMACaddress(WAN_port);
	
	Menu("default");

	/*Insert send buffer for compare receive buffer by define in lib_file.h*/
	InsertSendBuffer();

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

	pthread_join(pthreadSendPacket, NULL);
	pthread_join(pthreadReadLoop, NULL);
	pthread_join(pthreadReadLoopLAN, NULL);

	return 0;
}

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
	SBr.SendBuf = xmalloc(sizeof(DHCPdocsisBuf));
	SBr.ReceiveBuf = xmalloc(sizeof(DHCPdocsisBuf));

	for(s_dis = 18; s_dis < sizeof(DHCPdocsisBuf); s_dis++)
	{
		SBr.SendBuf[eth_ft++] = DHCPdocsisBuf[s_dis];
	}

	int s_pkt = 0, eth_pk = 0;
	SBr.SendpktcBuf = xmalloc(sizeof(DHCPpktcBuf));
	for(s_pkt = 18; s_pkt < sizeof(DHCPpktcBuf); s_pkt++)
	{
		SBr.SendpktcBuf[eth_pk++] = DHCPpktcBuf[s_pkt];
	}
	/*************************************************/

	/*store send DHCP offer buf in char for compare********/
	int s_off = 0, eth_ftoff = 0;
	SBr.SendBuf_offer = xmalloc(sizeof(DHCPdocsisBuf_offer));
	SBr.ReceiveBuf_offer = xmalloc(sizeof(DHCPdocsisBuf_offer));

	for(s_off = 14; s_off < sizeof(DHCPdocsisBuf_offer); s_off++)
	{
		SBr.SendBuf_offer[eth_ftoff++] = DHCPdocsisBuf_offer[s_off];
	}

	int s_poff = 0, eth_pkoff = 0;
	SBr.SendpktcBuf_offer = xmalloc(sizeof(DHCPpktcBuf_offer));
	for(s_poff = 14; s_poff < sizeof(DHCPpktcBuf_offer); s_poff++)
	{
		SBr.SendpktcBuf_offer[eth_pkoff++] = DHCPpktcBuf_offer[s_poff];
	}
	/*************************************************/

	/*Strore send ARP buf in char for compare*********/
	int s_arp = 0, eth_arp = 0;
	SBr.SendBuf_arp = xmalloc(sizeof(ArpPacket));
	SBr.ReceiveBuf_arp = xmalloc(sizeof(ArpPacket));

	for(s_arp = 18; s_arp < sizeof(ArpPacket); s_arp++)
	{
		SBr.SendBuf_arp[eth_arp++] = ArpPacket[s_arp];
	}
	/*************************************************/
	
	/*Strore send TFTP buf in char for compare*********/
	int s_tftp = 0, eth_tftp = 0;
	SBr.SendBuf_tftpdocsis = xmalloc(sizeof(tftpPacket_docsis));
	SBr.SendBuf_tftpemta = xmalloc(sizeof(tftpPacket_emta));
	SBr.ReceiveBuf_tftp = xmalloc(sizeof(tftpPacket_emta));

	for(s_tftp = 18; s_tftp < sizeof(tftpPacket_docsis); s_tftp++)
	{
		SBr.SendBuf_tftpdocsis[eth_tftp++] = tftpPacket_docsis[s_tftp];
	}

	s_tftp = 0;
	eth_tftp = 0;

	for(s_tftp = 18; s_tftp < sizeof(tftpPacket_emta); s_tftp++)
	{
		SBr.SendBuf_tftpemta[eth_tftp++] = tftpPacket_emta[s_tftp];
	}
	/*************************************************/
}

void MACandVIDplus()
{
	//mac
	if(((DHCPdocsisBuf[10] & 0xff ) << 8 | DHCPdocsisBuf[11] & 0xff) < 2048)
	{
		if((DHCPpktcBuf[11] & 0xff) == 255)
		{
			/*DHCP MAC Add*/
			DHCPdocsisBuf[10] += 0x01;
			DHCPpktcBuf[10] += 0x01;

			DHCPdocsisBuf[11] = 0x00;
			DHCPpktcBuf[11] = 0x00;
			
			/*DHCP Bootstrap Layer MAC Add*/
		/*	DHCPdocsisBuf[78] += 0x01;
			DHCPpktcBuf[78] += 0x01;

			DHCPdocsisBuf[79] = 0x00;
			DHCPpktcBuf[79] = 0x00;*/
									
			/*ARP MAC Add*/
			ArpPacket[10] += 0x01;
			ArpPacket[11] = 0x00;

			/*ARP SenderMAC Add*/
			ArpPacket[30] += 0x01;
			ArpPacket[31] = 0x00;

			SBr.SendBuf_arp[12] += 0x01;
			SBr.SendBuf_arp[13] = 0x00;

		}
		else
		{
			/*DHCP MAC Add*/
			DHCPdocsisBuf[11] += 0x01;
			DHCPpktcBuf[11] += 0x01;

			/*DHCP Bootstrap Layer MAC Add*/
		/*	DHCPdocsisBuf[79] += 0x01;
			DHCPpktcBuf[79] += 0x01;*/

			/*ARP MAC Add*/
			ArpPacket[11] += 0x01;
			/*ARP Sender MAC Add*/
			ArpPacket[31] += 0x01;
			SBr.SendBuf_arp[13] += 0x01;
		}
	}
	else
	{
		/*DHCP MAC the last two bytes set to zero*/
		DHCPpktcBuf[10] = 0x00;
		DHCPdocsisBuf[10] = 0X00;
		DHCPpktcBuf[11] = 0x00;
		DHCPdocsisBuf[11] = 0X00;

		/*DHCP Bootstrap Layer MAC the last two bytes set to zero*/
	/*	DHCPdocsisBuf[78] = 0x00;
		DHCPpktcBuf[78] = 0x00;
		DHCPdocsisBuf[79] = 0x00;
		DHCPpktcBuf[79] = 0x00;
	*/	
		/*ARP MAC the last two bytes set to zero*/
		ArpPacket[10] = 0x00;
		ArpPacket[11] = 0x00;
		
		/*ARP Sender MAC the last two bytes set to zero*/
		ArpPacket[30] = 0x00;
		ArpPacket[31] = 0x00;

		SBr.SendBuf_arp[12] = 0x00;
		SBr.SendBuf_arp[12] = 0x00;
	}

	//vlan tag
	//if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 2512 || ((DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktcBuf[15] & 0xff) < 2512)
	if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 4095 || ((DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktcBuf[15] & 0xff) < 4095)
	{
		if((DHCPpktcBuf[15] & 0xff) == 255 || (DHCPdocsisBuf[15] & 0xff) == 255)
		{	
			/*DHCP VLAN ID Add to next byte for keep plus*/
			DHCPdocsisBuf[14] += 0x01;
			DHCPpktcBuf[14] += 0x01;

			DHCPdocsisBuf[15] = 0x00;
			DHCPpktcBuf[15] = 0x00;
			
			/*ARP VLAN ID Add to next byte for keep plus*/
			ArpPacket[14] += 0x01;
			ArpPacket[15] += 0x00;
		}
		else
		{
			/*DHCP VLAN ID Add*/
			DHCPpktcBuf[15] += 0x01;
			DHCPdocsisBuf[15] += 0X01;
			
			/*ARP VLAN ID Add*/
			ArpPacket[15] += 0x01;
		}
	}
	else
	{
		/*DHCP VLAN ID the last two bytes set to default 2049*/
		DHCPpktcBuf[14] = 0x08;
		DHCPdocsisBuf[14] = 0X08;
		DHCPpktcBuf[15] = 0x01;
		DHCPdocsisBuf[15] = 0X01;
		
		/*ARP VLAN ID the last two bytes set to defalt 2049*/
		ArpPacket[14] = 0x08;
		ArpPacket[15] = 0x01;
	}
}

//Not in use
void GetEthMACaddress(char eth_port[])
{
	int fd;
	struct ifreq ifr;
	unsigned char *mac;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, eth_port, IFNAMSIZ - 1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	mac = (unsigned char*) ifr.ifr_hwaddr.sa_data;

	DHCPdocsisBuf_offer[6] = mac[0];
	DHCPdocsisBuf_offer[7] = mac[1];
	DHCPdocsisBuf_offer[8] = mac[2];
	DHCPdocsisBuf_offer[9] = mac[3];
	DHCPdocsisBuf_offer[10] = mac[4];
	DHCPdocsisBuf_offer[11] = mac[5];

	DHCPpktcBuf_offer[6] = mac[0];
	DHCPpktcBuf_offer[7] = mac[1];
	DHCPpktcBuf_offer[8] = mac[2];
	DHCPpktcBuf_offer[9] = mac[3];
	DHCPpktcBuf_offer[10] = mac[4];
	DHCPpktcBuf_offer[11] = mac[5];
}
