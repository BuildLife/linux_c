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

unsigned char Docsis_sidMAC[2048][10] = {0};
unsigned char Pktc_sidMAC[2048][10] = {0};

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
unsigned int CompareFalseTimes_tftp = 0, CompareTrueTimes_tftp = 0;
unsigned int CompareFalseTimes_Option82 = 0, CompareTrueTimes_Option82 = 0;

/* Socket Server for Auto testing flag ->  1 : open auto ; 0 : close auto */
static int AutoTesting = 0;

/*Record Lose packet*/
static int ReceiveDOCSISpkt = 0, ReceiveOFFERpkt = 0, ReceiveARPpkt = 0, ReceiveTFTPpkt = 0;
static int ReceiveDOCSISpkt_Option82 = 0;

/*Pthread lock*/
pthread_mutex_t pcap_send_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pcap_read_mutex = PTHREAD_MUTEX_INITIALIZER;

/*Sending Packet for DHCP or ARP*/
char *PacketMode = "default";

/*Option 82*/
char *Option82 = "disable";


static int StopLoopRunning = 0;

/*Ctrl+c change the values for stop read/send while*/
void Signal_Stophandler()
{
	StopLoopRunning = 1;
}

/*Client socket for use*/
void ThreadClientSocket()
{	
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
			
			if((client_buffer[0] != 0) && (client_buffer[5] != 0))
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
					
					/*Set TFTP VLAN ID*/
					tftpPacket_docsis[14] = (Start_VID >> 8) & 0xff;
					tftpPacket_docsis[15] = (Start_VID) & 0xff;

					tftpPacket_emta[14] = (Start_VID >> 8) & 0xff;
					tftpPacket_emta[15] = (Start_VID) & 0xff;
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
				
				if(client_buffer[5] == 1)
					Option82 = "enable";
				else if(client_buffer[5] == 2)
					Option82 = "disable";

				Option_ReceiveAndRunning(0, testmode);

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
	char cmd_buf;
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
			printf("Code Compares VID -> default : %d, docsis : %d, pktc : %d\n",default_vid, docsis_vid, pktc_vid);
			printf("Option82 (Y/N) [N] :\n");
			scanf("%c");
			cmd_buf = getchar();

			if(cmd_buf == 'y' || cmd_buf == 'Y')
			{
				printf("Option82 Enable:\n");
				Option82 = "enable";
			}
			else if(cmd_buf == 'N' || cmd_buf == 'n')
			{
				Option82 = "disable";
			}
			printf("Enter Send Times or Enter 0 keep sending loop:\n");
			printf("Sending Times : ");
			if(scanf("%u", &Running_Times) == 0)
			{
				printf("Enter the wrong word , Running Times set 1 time\n");
				Running_Times = 1;
			}
			if(Running_Times == 0)
			{
				/*Use in while loop always still Running*/
				KeepRunning = 1;

				/*It's for control stop while loop of Ctrl+c*/
				/*Avoid enter expect Ctrl+c, so in the start to set 0*/
				StopLoopRunning = 0;
			}

			printf("Enter Start VID(Range : 2049 ~ 2512):\n");
			printf("VID : ");
			if(scanf("%d", &Start_VID) != 1)
			{
				printf("Enter the wrong word, so the VID set 2049\n");
				Start_VID = 2049;
			}
			else if(Start_VID < 2049 || Start_VID > 2512)
			{
				printf("The Enter VID number out of the range , so the VID set 2049\n");
				Start_VID = 2049;
			}
			else if(Start_VID >= 2049 && Start_VID < 2512)
			{	
				/*Set DHCP VLAN ID*/
				DHCPdocsisBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPdocsisBuf[15] = (Start_VID) & 0xff;
				DHCPpktcBuf[14] = (Start_VID >> 8) & 0xff;
				DHCPpktcBuf[15] = (Start_VID) & 0xff;
				
				/*Set ARP VLAN ID*/
				ArpPacket[14] = (Start_VID >> 8) & 0xff;
				ArpPacket[15] = (Start_VID) & 0xff;
			
				/*Set TFTP VLAN ID*/
				tftpPacket_docsis[14] = (Start_VID >> 8) & 0xff;
				tftpPacket_docsis[15] = (Start_VID) & 0xff;

				tftpPacket_emta[14] = (Start_VID >> 8) & 0xff;
				tftpPacket_emta[15] = (Start_VID) & 0xff;
			
			}
		}
	}
}

//transformation mac address
char *MacaddrtoString(unsigned char *mac_d)
{
	static char MACSTR[MAC_ADDRSTRLEN];
	snprintf(MACSTR, sizeof(MACSTR), "%02x:%02x:%02x:%02x:%02x:%02x", mac_d[0], mac_d[1], mac_d[2], mac_d[3], mac_d[4], mac_d[5]);

	return MACSTR;
}


//Combinded SVGM and DVGM in this Function
void VGM_MODE(u_int32_t length, const u_int8_t *content)
{
	char *DumpIP_buffer;
	int DumpIP_length = 0;
	/********************************* For LAN buffer **************************************/
	//docsis buffer 
	char LAN_docsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_docsis_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_docsis_type;
	u_int16_t LAN_docsis_TPID;

	strlcpy(LAN_docsis_dstmac, MacaddrtoString(LAN_docsis_ethhdr -> dmac), sizeof(LAN_docsis_dstmac));
	strlcpy(LAN_docsis_srcmac, MacaddrtoString(LAN_docsis_ethhdr -> smac), sizeof(LAN_docsis_srcmac));
		
	LAN_docsis_type = ntohs(LAN_docsis_ethhdr -> type);

	LAN_docsis_TPID = ntohs(LAN_docsis_ethhdr -> tpid);

	//pktc buffer
	char LAN_pktc_dstmac[MAC_ADDRSTRLEN] = {}, LAN_pktc_srcmac[MAC_ADDRSTRLEN] = {};
	
	u_int16_t LAN_pktc_type;
	u_int16_t LAN_pktc_TPID;

	strlcpy(LAN_pktc_dstmac, MacaddrtoString(LAN_pktc_ethhdr -> dmac), sizeof(LAN_pktc_dstmac));
	strlcpy(LAN_pktc_srcmac, MacaddrtoString(LAN_pktc_ethhdr -> smac), sizeof(LAN_pktc_srcmac));
	
	LAN_pktc_type = ntohs(LAN_pktc_ethhdr -> type);
	
	LAN_pktc_TPID = ntohs(LAN_pktc_ethhdr -> tpid);
	
	//arp buffer
	char LAN_arp_dstmac[MAC_ADDRSTRLEN] = {}, LAN_arp_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_arp_type;
	u_int16_t LAN_arp_TPID;

	strlcpy(LAN_arp_dstmac, MacaddrtoString(LAN_arp_ethhdr -> dmac), sizeof(LAN_arp_dstmac));
	strlcpy(LAN_arp_srcmac, MacaddrtoString(LAN_arp_ethhdr -> smac), sizeof(LAN_arp_srcmac));

	LAN_arp_type = ntohs(LAN_arp_ethhdr -> type);

	LAN_arp_TPID = ntohs(LAN_arp_ethhdr -> tpid);

	//tftp buffer
	char LAN_tftpdocsis_dstmac[MAC_ADDRSTRLEN] = {}, LAN_tftpdocsis_srcmac[MAC_ADDRSTRLEN] = {};
	char LAN_tftpemta_dstmac[MAC_ADDRSTRLEN] = {}, LAN_tftpemta_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t LAN_tftp_type;
	u_int16_t LAN_tftp_TPID;
	
	strlcpy(LAN_tftpdocsis_dstmac, MacaddrtoString(LAN_tftpdocsis_ethhdr -> dmac), sizeof(LAN_tftpdocsis_dstmac));
	strlcpy(LAN_tftpdocsis_srcmac, MacaddrtoString(LAN_tftpdocsis_ethhdr -> smac), sizeof(LAN_tftpdocsis_srcmac));
	
	strlcpy(LAN_tftpemta_dstmac, MacaddrtoString(LAN_tftpemta_ethhdr -> dmac), sizeof(LAN_tftpemta_dstmac));
	strlcpy(LAN_tftpemta_srcmac, MacaddrtoString(LAN_tftpemta_ethhdr -> smac), sizeof(LAN_tftpemta_srcmac));

	LAN_tftp_type = ntohs(LAN_tftpdocsis_ethhdr -> type);

	LAN_tftp_TPID = ntohs(LAN_tftpdocsis_ethhdr -> tpid);

	/****************************************************************************************/

	/********************************* For WAN buffer ***************************************/
	eth_header *WAN_ethhdr = (eth_header*)content;

	//WAN : set & get source mac and destination mac
	char WAN_dstmac[MAC_ADDRSTRLEN] = {}, WAN_srcmac[MAC_ADDRSTRLEN] = {};

	u_int16_t WAN_type;
	u_int16_t WAN_TPID;

	//WAN : set mac 
	strlcpy(WAN_dstmac, MacaddrtoString(WAN_ethhdr -> dmac), sizeof(WAN_dstmac));
	strlcpy(WAN_srcmac, MacaddrtoString(WAN_ethhdr -> smac), sizeof(WAN_srcmac));
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
		
		if(ChangeMode == "DVGM")
		{
			DumpIP_buffer = (ip_header*)(content + sizeof(eth_header) - 4);

			if(Option82 == "enable")
				DumpIP_length = length - sizeof(eth_header) - OPTION82_LENGTH + 3; 
			else if(Option82 == "disable")
				DumpIP_length = length - sizeof(eth_header) + 4; 
		}
		else if(ChangeMode == "SVGM")
		{
			DumpIP_buffer = (ip_header*)(content + sizeof(eth_header));

			if(Option82 == "enable")
				DumpIP_length = length - sizeof(eth_header) - OPTION82_LENGTH - 1; 
			else if(Option82 == "disable")
				DumpIP_length = length - sizeof(eth_header); 
		}

	  if(DHCPBufMode == "docsis")
	  {
		printf("Destination 	: %17s        |   %17s\n",LAN_docsis_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_docsis_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_docsis_type,WAN_type);
		
	  	if(ChangeMode == "DVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c%c%c                |   %c%c%c%c%c%c%c%c%c\n",
							DHCPdocsisBuf[302], DHCPdocsisBuf[303], DHCPdocsisBuf[304], 
							DHCPdocsisBuf[305], DHCPdocsisBuf[306], DHCPdocsisBuf[307],
							DHCPdocsisBuf[308], DHCPdocsisBuf[309], DHCPdocsisBuf[310],
							content[298], content[299], content[300],
							content[301], content[302], content[303],
							content[304], content[305], content[306]);
		
			printf("802.1Q Virtual LAN ID : %u\n",LAN_docsis_TPID);
		}
		else if(ChangeMode == "SVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c%c%c                |   %c%c%c%c%c%c%c%c%c\n",
							DHCPdocsisBuf[302], DHCPdocsisBuf[303], DHCPdocsisBuf[304], 
							DHCPdocsisBuf[305], DHCPdocsisBuf[306], DHCPdocsisBuf[307],
							DHCPdocsisBuf[308], DHCPdocsisBuf[309], DHCPdocsisBuf[310],
							content[302], content[303], content[304],
							content[305], content[306], content[307],
							content[308], content[309], content[310]);

			printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_docsis_TPID,WAN_TPID);
			
		}	
	  }//DHCPBufMode == "docsis"
	  else if(DHCPBufMode == "pktc")
	  {
		printf("Destination 	: %17s        |   %17s\n",LAN_pktc_dstmac, WAN_dstmac);
	
		printf("Source      	: %17s        |   %17s\n",LAN_pktc_srcmac, WAN_srcmac);
	
		printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_pktc_type,WAN_type);
		
	  	if(ChangeMode == "DVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c                  |   %c%c%c%c%c%c%c\n",
							DHCPpktcBuf[302], DHCPpktcBuf[303], DHCPpktcBuf[304], 
							DHCPpktcBuf[305], DHCPpktcBuf[306], DHCPpktcBuf[307],
							DHCPpktcBuf[308],
							content[298], content[299], content[300],
							content[301], content[302], content[303], content[304]);
		
			printf("802.1Q Virtual LAN ID : %u\n", LAN_pktc_TPID);
			
		}
		else if(ChangeMode == "SVGM")
		{
			printf("Option 60 class : %c%c%c%c%c%c%c                  |   %c%c%c%c%c%c%c\n",
							DHCPpktcBuf[302], DHCPpktcBuf[303], DHCPpktcBuf[304], 
							DHCPpktcBuf[305], DHCPpktcBuf[306], DHCPpktcBuf[307],
							DHCPpktcBuf[308],
							content[302], content[303], content[304],
							content[305], content[306], content[307], content[308]);
		
			printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_pktc_TPID,WAN_TPID);
	  	
		}
	  } //if DHCPBufMode == "pktc" 

		/*Sending buffer to dump ip and option82 function*/
		dump_DHCP_ip(DumpIP_buffer, DumpIP_length, WAN_ethhdr -> smac, WAN_TPID);

		//clean buffer to 0
		memset(DumpIP_buffer, 0, DumpIP_length);

	}//if PacketMode == "DHCP"
	else if(PacketMode == "ARP")
	{
		printf("------------------------------- ARP Packet ---------------------------------------\n");
		
		printf("Destination 	: %17s        |   %17s\n",LAN_arp_dstmac, WAN_dstmac);
		
		printf("Source      	: %17s        |   %17s\n",LAN_arp_srcmac, WAN_srcmac);
	  
	  if(ChangeMode == "DVGM")
	  {
		printf("Sender IP      	: %d.%d.%d.%d           |   %d.%d.%d.%d\n",
							ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, 
							content[28]%0xff, content[29]%0xff, content[30]%0xff, content[31]%0xff);
		
		printf("Tender IP      	: %d.%d.%d.%d             |   %d.%d.%d.%d\n",
							ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, 
							content[38]&0xff, content[39]&0xff, content[40]&0xff, content[41]&0xff);
		
		printf("802.1Q Virtual LAN ID : %u\n", LAN_arp_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
	  }
	  else if(ChangeMode == "SVGM")
	  {
		printf("Sender IP      	: %u.%u.%u.%u           |   %u.%u.%u.%u\n",
							ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff, 
							content[32]&0xff, content[33]&0xff, content[34]&0xff, content[35]&0xff);
		
		printf("Tender IP      	: %u.%u.%u.%u             |   %u.%u.%u.%u\n",
							ArpPacket[42]&0xff, ArpPacket[43]&0xff, ArpPacket[44]&0xff, ArpPacket[45]&0xff, 
							content[42]&0xff, content[43]&0xff, content[44]&0xff, content[45]&0xff);
		
		printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_arp_TPID, WAN_TPID);
		
		dump_ARP_ip((arp_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));
	  }
	}//if PacketMode == "ARP"
	else if(PacketMode == "TFTP")
	{
	  	printf("------------------------------- TFTP Packet ---------------------------------------\n");
		//printf("Ethernet Type 	: 0x%04x                   |   0x%04x\n",LAN_tftp_type, WAN_type);
	  	if(ChangeMode == "DVGM")
	  	{
			if(content[19] == 0x06){
			printf("Destination 	: %17s        |   %17s\n",LAN_tftpdocsis_dstmac, WAN_dstmac);
		
			printf("Source      	: %17s        |   %17s\n",LAN_tftpdocsis_srcmac, WAN_srcmac);
			printf("Ask Files Name  : %c%c%c%c%c%c%c%c		   |   %c%c%c%c%c%c%c\n", 
								tftpPacket_docsis[48], tftpPacket_docsis[49], tftpPacket_docsis[50], 
								tftpPacket_docsis[51], tftpPacket_docsis[52], tftpPacket_docsis[53],
								tftpPacket_docsis[54], tftpPacket_docsis[55], 
								content[44], content[45], content[46], content[47], 
								content[48], content[49], content[50], content[51]);
			}
			else if(content[19] == 0x17) {
			printf("Destination 	: %17s        |   %17s\n",LAN_tftpemta_dstmac, WAN_dstmac);
		
			printf("Source      	: %17s        |   %17s\n",LAN_tftpemta_srcmac, WAN_srcmac);
		
			printf("Ask Files Name  : %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c	   |	  %c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 
								tftpPacket_emta[48], tftpPacket_emta[49], tftpPacket_emta[50], 
								tftpPacket_emta[51], tftpPacket_emta[52], tftpPacket_emta[53],
								tftpPacket_emta[54], tftpPacket_emta[55], tftpPacket_emta[56], 
								tftpPacket_emta[57], tftpPacket_emta[58], tftpPacket_emta[59], 
								tftpPacket_emta[60], tftpPacket_emta[61], tftpPacket_emta[62], 
								content[44], content[45], content[46], content[47], 
								content[48], content[49], content[50], content[51],
								content[52], content[53], content[54], content[55],
								content[56], content[57], content[58]);
			}
			printf("802.1Q Virtual LAN ID : %u\n",LAN_tftp_TPID);
	  
			dump_TFTP_ip((ip_header*)(content + sizeof(eth_header) - 4), length - sizeof(eth_header) - 4);
	  	}
	  	else if(ChangeMode == "SVGM")
	  	{
			if(content[23] == 0x06){
			printf("Destination 	: %17s        |   %17s\n",LAN_tftpdocsis_dstmac, WAN_dstmac);
		
			printf("Source      	: %17s        |   %17s\n",LAN_tftpdocsis_srcmac, WAN_srcmac);
		
			printf("Ask Files Name  : %c%c%c%c%c%c%c%c		   |   %c%c%c%c%c%c%c\n", 
								tftpPacket_docsis[48], tftpPacket_docsis[49], tftpPacket_docsis[50], 
								tftpPacket_docsis[51], tftpPacket_docsis[52], tftpPacket_docsis[53],
								tftpPacket_docsis[54], tftpPacket_docsis[55], 
								content[48], content[49], content[50], content[51], 
								content[52], content[53], content[54], content[55]);
			}
			else if(content[23] == 0x17) {
			printf("Destination 	: %17s        |   %17s\n",LAN_tftpemta_dstmac, WAN_dstmac);
		
			printf("Source      	: %17s        |   %17s\n",LAN_tftpemta_srcmac, WAN_srcmac);
		
			printf("Ask Files Name  : %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c	   |   %c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 
								tftpPacket_emta[48], tftpPacket_emta[49], tftpPacket_emta[50], 
								tftpPacket_emta[51], tftpPacket_emta[52], tftpPacket_emta[53],
								tftpPacket_emta[54], tftpPacket_emta[55], tftpPacket_emta[56], 
								tftpPacket_emta[57], tftpPacket_emta[58], tftpPacket_emta[59], 
								tftpPacket_emta[60], tftpPacket_emta[61], tftpPacket_emta[62], 
								content[48], content[49], content[50], content[51], 
								content[52], content[53], content[54], content[55],
								content[56], content[57], content[58], content[59],
								content[60], content[61], content[62]);
			}
			printf("802.1Q Virtual LAN ID : %u               |   %u\n",LAN_tftp_TPID,WAN_TPID);
	  		
			dump_TFTP_ip((ip_header*)(content + sizeof(eth_header)), length - sizeof(eth_header));
	  	}
	}//PacketMode == "TFTP"
}
void dump_DHCP_ip(ip_header *ipv4, int length, char mac[], unsigned int tpid)
{
		/*for discover buffer*/
		int compare_num = 0;
		int i = 0, x = 0;
		int i_82 = 28, x_82 = 28;

		/*for offer buffer*/
		int compare_offer = 0;
		int i_offer = 0, x_offer = 0;
		
		if(Option82 == "enable"){
			i = i_82;
			x = x_82;
		}
		else if(Option82 == "disable"){
			i = 0;
			x = 0;
		}
			

		if(htons(ipv4 -> ip_id) == 0xdead)
		{
			//Record receive docsis packet
			ReceiveDOCSISpkt++;

			SBr.ReceiveBuf = (char*)ipv4;
			
			if(DHCPBufMode == "docsis")
			{
				for(;i < length; i++)
				{
					if(SBr.SendBuf[i] != SBr.ReceiveBuf[i])
					{
						compare_num += 1;
					}
				}
			}
			else if(DHCPBufMode == "pktc")
			{
				for(;x < length; x++)
				{
					if(SBr.SendpktcBuf[x] != SBr.ReceiveBuf[x])
					{
						compare_num += 1;
					}
				}
				printf("\n");
			}
			
			if(compare_num > 0)
				CompareFalseTimes += 1;
			else
				CompareTrueTimes += 1;
				
			//Sending buffer to Option82 function --> if option82 state in open.
			if(Option82 == "enable")
			{
				char *Option82_buffer = (char*)(SBr.ReceiveBuf + length);
				CompareFalseTimes_Option82 = Option82_Compare(DHCPBufMode, mac, tpid, &CompareTrueTimes_Option82, Option82_buffer, OPTION82_LENGTH);
			}

			printf("\n");
			printf("***************** Compare Data **********************************\n");
			printf(" '%s' Compare data --------> %s\n", DHCPBufMode, !compare_num ? "true" : "false");
			printf("*****************************************************************\n");
			/*Clear ReceiveBuf to zero*/
			memset(SBr.ReceiveBuf, 0, length);
			
			/*for test   -->  if DHCP buffer compare false after stop this loop*/
			if((!compare_num) == 0)
				StopLoopRunning = 1;

		}//htons(ipv4 -> ip_id)
		else if(htons(ipv4 -> ip_sum) == 0xa381 || htons(ipv4 -> ip_sum) == 0xa3ef)
		{
			//Record receive pktc packet
			ReceiveOFFERpkt++;

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
			printf("********** Compare Data *************\n");
			printf(" '%s offer' Compare data --------> %s\n", DHCPBufMode,!compare_offer ? "true" : "false");
			printf("*************************************\n");
			
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

	/*According to receive buf and send buf all have sender ip to compare.*/
	//Avoid the others data mix in it.
	if(!strcmp(send_sender_ip, receive_sender_ip)) {
		//Record Receive packet 
		ReceiveARPpkt++;

		SBr.ReceiveBuf_arp = (char*)arp_ipv4;
		
		for(i = 0; i < length; i++)
		{
			if(SBr.SendBuf_arp[i] != SBr.ReceiveBuf_arp[i])
				compare_arp += 1;
		}

		printf("\n");
		printf("******* ARP Compare Data *******\n");
		printf(" 'ARP' Compare data --------> %s\n", !compare_arp ? "true" : "false");
		printf("********************************\n");
				
		if(compare_arp > 0)
			CompareFalseTimes_arp += 1;
		else
			CompareTrueTimes_arp += 1;

	  	memset(SBr.ReceiveBuf_arp, 0, length);
	  }
}

void dump_TFTP_ip(ip_header *tftp_ipv4, int length)
{
	int compare_tftpdocsis = 0, compare_tftpemta = 0;
	int i_tftp = 0, j_tftp = 0;

	if(htons(tftp_ipv4 -> ip_id) == 0x0006)
	{
		ReceiveTFTPpkt++;

		SBr.ReceiveBuf_tftp = (char*)tftp_ipv4;
		
		for(i_tftp = 0; i_tftp < length; i_tftp++)
		{
			if(SBr.ReceiveBuf_tftp[i_tftp] != SBr.SendBuf_tftpdocsis[i_tftp])
				compare_tftpdocsis += 1;
		}

		printf("\n");
		printf("********** TFTP Compare Data ***********\n");
		printf(" 'TFTP DOCSIS' Compare data --------> %s\n", !compare_tftpdocsis ? "true" : "false");
		printf("****************************************\n");
		
		
		if(compare_tftpdocsis > 0)
			CompareFalseTimes_tftp += 1;
		else
			CompareTrueTimes_tftp += 1;

		memset(SBr.ReceiveBuf_tftp, 0, length);
	}
	else if(htons(tftp_ipv4 -> ip_id) == 0x0017)
	{
		ReceiveTFTPpkt++;

		SBr.ReceiveBuf_tftp = (char*)tftp_ipv4;

		for(j_tftp = 0; j_tftp < length; j_tftp++)
		{
			if(SBr.ReceiveBuf_tftp[j_tftp] != SBr.SendBuf_tftpemta[j_tftp])
				compare_tftpemta += 1;
		}

		printf("\n");
		printf("******** TFTP Compare Data ***********\n");
		printf(" 'TFTP EMTA' Compare data --------> %s\n", !compare_tftpemta ? "true" : "false");
		printf("**************************************\n");

		if(compare_tftpemta > 0)
			CompareFalseTimes_tftp += 1;
		else
			CompareTrueTimes_tftp += 1;
		
		memset(SBr.ReceiveBuf_tftp, 0, length);
	}
}


/*  In lib_file.h defined struct default_vid = 10; docsis_vid = 20; pktc_vid = 30; 
	that be compare vid when receive buffer(docsis/pktc) of SVGM Mode. 
	If needed to compare right vid. Have to change the values in lib_file.h */
int Option82_Compare(char *DHCPmode, char MAC[], unsigned int VID, int *Compares, char *Option82_content, unsigned int Option82_content_length)
{
	
	static int false_times;
	static int ReceiveTimes;
	static int o_flag;

	ReceiveDOCSISpkt_Option82++;

	printf("******************* Option 82 Information *********************\n");
	printf("Circuit ID : %c%c%c%c%c%c\n", Option82_content[4], Option82_content[5], Option82_content[6], Option82_content[7], Option82_content[8], Option82_content[9]);
	printf("Remote ID : %02x:%02x:%02x:%02x:%02x:%02x\n", Option82_content[12] & 0xff, Option82_content[13] & 0xff, Option82_content[14] & 0xff, Option82_content[15] & 0xff, Option82_content[16] & 0xff, Option82_content[17] & 0xff);
	printf("***************************************************************\n");
	
	int i = 0, j = 0;
	char item_docsis[6] = {0};
	char item_pktc[6] = {0};

	if(ChangeMode == "SVGM")
	{
		if(DHCPmode == "docsis")
		{
			memcpy(item_docsis, *(Docsis_sidMAC + ReceiveTimes) + 4, sizeof(MAC));
			
			if(!strcmp(item_docsis, MAC))
			{
				if(VID != docsis_vid)
					false_times += 1;
				else
					*Compares += 1;
			}
			else
			{
				printf("Error MAC---------------------------------------\n");
				false_times += 1;
			}
			o_flag = 0;
		}
		else if(DHCPmode == "pktc")
		{
			memcpy(item_pktc, *(Pktc_sidMAC + ReceiveTimes) + 4, sizeof(MAC));
			if(!strcmp(item_pktc, MAC))
			{
				if(VID != pktc_vid)
					false_times += 1;
				else
					*Compares += 1;
			}	
			else
			{
				printf("Error MAC---------------------------------------\n");
				false_times += 1;
			}
			o_flag = 1;
		}
	}

	if(o_flag == 1)
	{
		ReceiveTimes++;
	}

	if(ReceiveTimes == 255)
	{
		ReceiveTimes = 0;
	}
	return false_times;
}


/*WAN Port read Function*/
void pcap_handler_func(unsigned char *user,const struct pcap_pkthdr *header, const unsigned char *bytes)
{

	//check buffer length is enough normal ethernet buffer
	if( header -> caplen < sizeof(ip_header) + sizeof(struct ether_header))
	{
		return;
	}

	char send_sender_ip[16] = {0};
	char receive_sender_ip[16] = {0};
	sprintf(send_sender_ip, "%d.%d.%d.%d", ArpPacket[32]&0xff, ArpPacket[33]&0xff, ArpPacket[34]&0xff, ArpPacket[35]&0xff);

	//Identification receive was the same buffer
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
		printf("Current Send Times : %s", GetLocalTimetoString(&header -> ts.tv_sec));
		printf("\n");
		printf("-----------------------------------------------------------------------------------------------\n");
		printf("\n");
		VGM_MODE(header -> caplen, bytes);
	}
	else if(PacketMode == "ARP" && !strcmp(send_sender_ip, receive_sender_ip))
	{
		printf("Current Send Times : %s", GetLocalTimetoString(&header -> ts.tv_sec));
		VGM_MODE(header -> caplen, bytes);
	}
	else if(PacketMode == "TFTP")
	{
		if(BufferIdentification == 0x0006 || BufferIdentification == 0x0017)
		{
			printf("Current Send Times : %s", GetLocalTimetoString(&header -> ts.tv_sec));
			VGM_MODE(header -> caplen, bytes);
		}
	}
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

	/********************************* For LAN buffer ***************************************/
	char WAN_docsis_dstmac_offer[MAC_ADDRSTRLEN] = {}, WAN_docsis_srcmac_offer[MAC_ADDRSTRLEN] = {};
	char WAN_pktc_dstmac_offer[MAC_ADDRSTRLEN] = {}, WAN_pktc_srcmac_offer[MAC_ADDRSTRLEN] = {};

	u_int16_t WAN_docsis_type;
	u_int16_t WAN_pktc_type;

	//LAN : set mac 
	strlcpy(WAN_docsis_dstmac_offer, MacaddrtoString(WAN_docsis_ethhdr_offer -> dmac), sizeof(WAN_docsis_dstmac_offer));
	strlcpy(WAN_docsis_srcmac_offer, MacaddrtoString(WAN_docsis_ethhdr_offer -> smac), sizeof(WAN_docsis_srcmac_offer));
	strlcpy(WAN_pktc_dstmac_offer, MacaddrtoString(WAN_pktc_ethhdr_offer -> dmac), sizeof(WAN_pktc_dstmac_offer));
	strlcpy(WAN_pktc_srcmac_offer, MacaddrtoString(WAN_pktc_ethhdr_offer -> smac), sizeof(WAN_pktc_srcmac_offer));

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
	strlcpy(LAN_dstmac_offer, MacaddrtoString(LAN_ethhdr_offer -> dmac), sizeof(LAN_dstmac_offer));
	strlcpy(LAN_srcmac_offer, MacaddrtoString(LAN_ethhdr_offer -> smac), sizeof(LAN_srcmac_offer));
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
		printf("Current Send Times : %s", GetLocalTimetoString(&header -> ts.tv_sec));
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

		dump_DHCP_ip((ip_header*)(bytes + sizeof(eth_header) - 4), header -> caplen - sizeof(eth_header) - 4, WAN_docsis_dstmac_offer, 0);
	}	
}

/*Use to filter UDP or ARP buffer and keep reading WAN port buffer*/
void read_loop()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *p_read;

	struct bpf_program bpf_p;
	bpf_u_int32 net, mask;

	char protocol_filter[8];

	if(PacketMode == "DHCP" || PacketMode == "TFTP")
		sprintf(protocol_filter, "%s", "udp");
	else if(PacketMode == "ARP")
		sprintf(protocol_filter, "%s", "arp");

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


void Option_ReceiveAndRunning(int D_times, char sop)
{
	/*Random Sending data*/
	int Random_send = 0;

	/*Sums sends packet times*/
	int docsis_sendtimes = 0;
	int arp_sendtimes = 0;
	int tftp_sendtimes = 0;
	int offer_sendtimes = 0;

	char errbuf[PCAP_ERRBUF_SIZE];

	/*LAN Port pcap send*/
	pcap_t *p_lan;
	
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
		//	Random_send = (rand() % 100)+1;
			D_times++;

			printf("Send Times --------------> %d\n", D_times);
			/*********************** Sending Docsis packet ******************************/
				DHCPBufMode = "docsis";

				pthread_mutex_lock(&pcap_send_mutex);
					

				if(pcap_sendpacket(p_lan, DHCPdocsisBuf, sizeof(DHCPdocsisBuf)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					SBr.SendBuf_arp[11] = 0x11;
					ArpPacket[9] = 0x11;
					ArpPacket[29] = 0x11;
					ArpPacket[35] = 0x20;
					docsis_sendtimes++;
					PacketMode = "DHCP";
					if(Option82 == "enable")
					{
						InsertMACTable("docsis", LAN_docsis_ethhdr -> smac);
					}
				}
				nanosleep(&send_ts, NULL);

				//Sending ARP Packet
				if(pcap_sendpacket(p_lan, ArpPacket, sizeof(ArpPacket)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					arp_sendtimes++;
					PacketMode = "ARP";
				}
				nanosleep(&send_ts, NULL);

				//Sending TFTP Packet
				if(pcap_sendpacket(p_lan, tftpPacket_docsis, sizeof(tftpPacket_docsis)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					tftp_sendtimes++;
					PacketMode = "TFTP";
				}
				nanosleep(&send_ts, NULL);

				if(pcap_sendpacket(p_wan, DHCPdocsisBuf_offer, sizeof(DHCPdocsisBuf_offer)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					offer_sendtimes++;
				}	

				pthread_mutex_unlock(&pcap_send_mutex);

				sleep(1);

			/********************** Sending EMTA packet ********************************/
				DHCPBufMode = "pktc";

				pthread_mutex_lock(&pcap_send_mutex);
					

				if(pcap_sendpacket(p_lan, DHCPpktcBuf, sizeof(DHCPpktcBuf)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					SBr.SendBuf_arp[11] = 0x22;
					ArpPacket[9] = 0x22;
					ArpPacket[29] = 0x22;
					ArpPacket[35] = 0x84;
					docsis_sendtimes++;
					PacketMode = "DHCP";
					if(Option82 == "enable")
					{
						InsertMACTable("pktc", LAN_pktc_ethhdr -> smac);
					}
				}
				nanosleep(&send_ts, NULL);
		
				//Sending ARP Packet
				if(pcap_sendpacket(p_lan, ArpPacket, sizeof(ArpPacket)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					arp_sendtimes++;
					PacketMode = "ARP";
				}
				nanosleep(&send_ts, NULL);

				//Sending TFTP Packet
				if(pcap_sendpacket(p_lan, tftpPacket_emta, sizeof(tftpPacket_emta)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_lan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					tftp_sendtimes++;
					PacketMode = "TFTP";
				}

				nanosleep(&send_ts, NULL);

				if(pcap_sendpacket(p_wan, DHCPpktcBuf_offer, sizeof(DHCPpktcBuf_offer)) < 0){
					fprintf(stderr, "pcap_sendpacket:%s\n", pcap_geterr(p_wan));
					pthread_mutex_unlock(&pcap_send_mutex);
					return;
				}
				else
				{
					offer_sendtimes++;
				}	

				pthread_mutex_unlock(&pcap_send_mutex);
			
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

		printf("*************** Current Mode : %s ****************\n", ChangeMode);
		printf("--------LAN  ==> ==> ==> ==> ==> ==>   WAN--------\n");
		printf("................... Discover .....................\n");
		printf("Discover -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes);
		printf("True  : %d\n", CompareTrueTimes);
		printf("Discover -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", docsis_sendtimes - ReceiveDOCSISpkt);
		if(Option82 == "enable" && ChangeMode == "SVGM")
		{
			printf("-------- SVGM Mode Option 82 VID Compare ---------\n");
			printf("Option82 Discover -> Compare Send packet and Receive packet\n");
			printf("VID False : %d\n", CompareFalseTimes_Option82);
			printf("VID True  : %d\n", CompareTrueTimes_Option82);
			printf("Option82 Discover -> Not arrive receive Port packet\n");
			printf("Lose Packet : %d\n", docsis_sendtimes - ReceiveDOCSISpkt_Option82);
		}
		printf(".................... ARP .........................\n");
		printf("ARP -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes_arp);
		printf("True  : %d\n", CompareTrueTimes_arp);
		printf("ARP -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", arp_sendtimes - ReceiveARPpkt);
		printf(".................... TFTP ........................\n");
		printf("TFTP -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes_tftp);
		printf("True  : %d\n", CompareTrueTimes_tftp);
		printf("TFTP -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", tftp_sendtimes - ReceiveTFTPpkt);
		printf("\n");
		printf("\n");
		printf("--------LAN  <== <== <== <== <== <==   WAN--------\n");
		printf("OFFER -> Compare Send packet and Receive packet\n");
		printf("False : %d\n", CompareFalseTimes_offer);
		printf("True  : %d\n", CompareTrueTimes_offer);
		printf("OFFER -> Not arrive receive Port packet\n");
		printf("Lose Packet : %d\n", offer_sendtimes - ReceiveOFFERpkt);
		printf("***************************************************\n");
		printf("\n");

		/***** Init default value *****/
		CompareFalseTimes = 0;
		CompareTrueTimes = 0;
		CompareFalseTimes_offer = 0;
		CompareTrueTimes_offer = 0;
		CompareTrueTimes_arp = 0;
		CompareFalseTimes_arp = 0;
		CompareTrueTimes_tftp = 0;
		CompareFalseTimes_tftp = 0;
		CompareTrueTimes_Option82 = 0;
		CompareFalseTimes_Option82 = 0;

		//Init Lose packet record count
		ReceiveDOCSISpkt = 0;
		ReceiveOFFERpkt = 0;
		ReceiveARPpkt = 0;
		ReceiveTFTPpkt = 0;
		ReceiveDOCSISpkt_Option82 = 0;

		KeepRunning = 0;
		Running_Times = 0;
		AutoTesting = 0;
		StopLoopRunning = 0;
		D_times = 0;
		Menu("default");

		Start_VID = 2049;
		/******************************/
			
		pcap_close(p_wan);
		pcap_close(p_lan);
}


void Command_Option()
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
			Option_ReceiveAndRunning(DHCPtimes, Cmdbuf);
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
			Command_Option();
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

	pth_send = pthread_create(&pthreadSendPacket, NULL, (void*)Command_Option, NULL);
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
	printf("\n");
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
									
			/*ARP MAC Add*/
			ArpPacket[10] += 0x01;
			ArpPacket[11] = 0x00;

			/*ARP SenderMAC Add*/
			ArpPacket[30] += 0x01;
			ArpPacket[31] = 0x00;

//			SBr.SendBuf_arp[12] += 0x01;
//			SBr.SendBuf_arp[13] = 0x00;

			/*TFTP MAC add*/
			tftpPacket_docsis[10] += 0x01;
			tftpPacket_emta[10] += 0x01;

			tftpPacket_docsis[11] = 0x00;
			tftpPacket_emta[11] = 0x00;

		}
		else
		{
			/*DHCP MAC Add*/
			DHCPdocsisBuf[11] += 0x01;
			DHCPpktcBuf[11] += 0x01;

			/*ARP MAC Add*/
			ArpPacket[11] += 0x01;
			/*ARP Sender MAC Add*/
			ArpPacket[31] += 0x01;
//			SBr.SendBuf_arp[13] += 0x01;

			/*TFTP MAC Add*/
			tftpPacket_docsis[11] += 0x01;
			tftpPacket_emta[11] += 0x01;
		}
	}
	else
	{
		/*DHCP MAC the last two bytes set to zero*/
		DHCPpktcBuf[10] = 0x00;
		DHCPdocsisBuf[10] = 0X00;
		DHCPpktcBuf[11] = 0x00;
		DHCPdocsisBuf[11] = 0X00;

		/*ARP MAC the last two bytes set to zero*/
		ArpPacket[10] = 0x00;
		ArpPacket[11] = 0x00;
		
		/*ARP Sender MAC the last two bytes set to zero*/
		ArpPacket[30] = 0x00;
		ArpPacket[31] = 0x00;

	//	SBr.SendBuf_arp[12] = 0x00;
//		SBr.SendBuf_arp[12] = 0x00;
		
		/*TFTP MAC Add*/	
		tftpPacket_docsis[10] = 0x00;
		tftpPacket_emta[10] = 0x00;
		tftpPacket_docsis[10] = 0x00;
		tftpPacket_emta[10] = 0x00;
	}

	//vlan tag
	//if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 2512 || ((DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktcBuf[15] & 0xff) < 2512)
	if(((DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff) < 4095 || ((DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktcBuf[15] & 0xff) < 4095)
	{
		if((DHCPpktcBuf[15] & 0xff) == 255 || (DHCPdocsisBuf[15] & 0xff) == 255 || (ArpPacket[15] & 0xff) == 255)
		{	
			/*DHCP VLAN ID Add to next byte for keep plus*/
			DHCPdocsisBuf[14] += 0x01;
			DHCPpktcBuf[14] += 0x01;

			DHCPdocsisBuf[15] = 0x00;
			DHCPpktcBuf[15] = 0x00;
			
			/*ARP VLAN ID Add to next byte for keep plus*/
			ArpPacket[14] += 0x01;
			ArpPacket[15] += 0x00;
			
			/*TFTP VLAN ID Add to next byte for keep plus*/
			tftpPacket_docsis[14] += 0x01;
			tftpPacket_emta[14] += 0x01;

			tftpPacket_docsis[15] = 0x00;
			tftpPacket_emta[15] = 0x00;
		}
		else
		{
			/*DHCP VLAN ID Add*/
			DHCPpktcBuf[15] += 0x01;
			DHCPdocsisBuf[15] += 0X01;
			
			/*ARP VLAN ID Add*/
			ArpPacket[15] += 0x01;

			/*TFTP VLAN ID Add*/
			tftpPacket_docsis[15] += 0x01;
			tftpPacket_emta[15] += 0x01;
		}
	}
	else
	{
		/*DHCP VLAN ID the last two bytes set to default 2049*/
		DHCPpktcBuf[14] = 0x08;
		DHCPdocsisBuf[14] = 0X08;
		DHCPpktcBuf[15] = 0x01;
		DHCPdocsisBuf[15] = 0X01;
		
		/*ARP VLAN ID the last two bytes set to default 2049*/
		ArpPacket[14] = 0x08;
		ArpPacket[15] = 0x01;
			
		/*TFTP VLAN ID the last two bytes set to default 2049*/
		tftpPacket_docsis[14] = 0x08;
		tftpPacket_emta[14] = 0x08;
		tftpPacket_docsis[15] = 0x01;
		tftpPacket_emta[15] = 0x01;
	}
}

/*For get WAN Port mac*/
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

	tftpPacket_docsis[0] = mac[0];
	tftpPacket_docsis[1] = mac[1];
	tftpPacket_docsis[2] = mac[2];
	tftpPacket_docsis[3] = mac[3];
	tftpPacket_docsis[4] = mac[4];
	tftpPacket_docsis[5] = mac[5];

	tftpPacket_emta[0] = mac[0];
	tftpPacket_emta[1] = mac[1];
	tftpPacket_emta[2] = mac[2];
	tftpPacket_emta[3] = mac[3];
	tftpPacket_emta[4] = mac[4];
	tftpPacket_emta[5] = mac[5];
}


int InsertMACTable(char *DhcpBufMode, char *MAC)
{
	static int def_sid_number;
	static char def_sid_title = 'A';
	static int flag = 0;
	static int items = 0;
	int docsis_vid = 0;
	int pktc_vid = 0;
	/*
	docsis_vid = (DHCPdocsisBuf[14] & 0xff) << 8 | DHCPdocsisBuf[15] & 0xff;
	pktc_vid = (DHCPpktcBuf[14] & 0xff) << 8 | DHCPpktc[15] & 0xff;
	*/

	if(def_sid_number == 0xff)
	{
		def_sid_number = 0x00;
		def_sid_title += 1;
	}
	int i = 0, j = 0;

	if(DhcpBufMode == "docsis")
	{
		strcpy(*(Docsis_sidMAC + items) + 0, &def_sid_title);
		Docsis_sidMAC[items][1] = def_sid_number;

		//add packet vid
		strcpy(*(Docsis_sidMAC + items) + 2, &DHCPdocsisBuf[14]);
		strcpy(*(Docsis_sidMAC + items) + 3, &DHCPdocsisBuf[15]);

		memcpy(*(Docsis_sidMAC + items) + 4, MAC, 6);
		flag = 0;
	}
	else if(DhcpBufMode == "pktc")
	{
		strcpy(*(Pktc_sidMAC + items) + 0, &def_sid_title);
		Pktc_sidMAC[items][1] = def_sid_number;
		
		//add packet vid
		strcpy(*(Pktc_sidMAC + items) + 2, &DHCPpktcBuf[14]);
		strcpy(*(Pktc_sidMAC + items) + 3, &DHCPpktcBuf[15]);
		
		memcpy(*(Pktc_sidMAC + items) + 4, MAC, 6);
		flag = 1;
	}

	if(flag == 1)
	{
		def_sid_number += 1;
		items += 1;
	}

	return sizeof(Docsis_sidMAC);
}

char *GetLocalTimetoString(const time_t *sec)
{
	char timebuf[64];
	memset(timebuf, 0, sizeof(timebuf));
	ctime_r(sec, timebuf);

	return timebuf;
}


