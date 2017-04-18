#include "lib_file.h"



pthread_t arpkeepsending;




void SendingArp()
{
	ArpPacket[6] = 0xc4;
	ArpPacket[7] = 0x12;
	ArpPacket[8] = 0xf5;
	ArpPacket[9] = 0x30;
	ArpPacket[10] = 0x92;
	ArpPacket[11] = 0xf9;
	
	ArpPacket[26] = 0xc4;
	ArpPacket[27] = 0x12;
	ArpPacket[28] = 0xf5;
	ArpPacket[29] = 0x30;
	ArpPacket[30] = 0x92;
	ArpPacket[31] = 0xf9;
	
	ArpPacket[0] = 0x08;
	ArpPacket[1] = 0x00;
	ArpPacket[2] = 0x27;
	ArpPacket[3] = 0x63;
	ArpPacket[4] = 0xa8;
	ArpPacket[5] = 0xde;
	
	ArpPacket[36] = 0x08;
	ArpPacket[37] = 0x00;
	ArpPacket[38] = 0x27;
	ArpPacket[39] = 0x63;
	ArpPacket[40] = 0xa8;
	ArpPacket[41] = 0xde;
	
	ArpPacket[34] = 0x0a;
	ArpPacket[35] = 0x02;
	ArpPacket[44] = 0x01;
	ArpPacket[45] = 0x63;

	static int times = 0;
	struct timespec send_ts;
	send_ts.tv_sec = 0;
	send_ts.tv_nsec = 500000000;
	
	char errBuf[PCAP_ERRBUF_SIZE];

	char *ethsend = "eth2";
	pcap_t *pSendarp;

	pSendarp = pcap_open_live(ethsend, 65536, 1, 0, errBuf);

	if(pSendarp == NULL)
	{
		fprintf(stderr,"Couldn't find the device %s\n",errBuf);
		return;
	}

	while(1)
	{
		if(pcap_sendpacket(pSendarp, ArpPacket, sizeof(ArpPacket)) < 0){
			fprintf(stderr,"pcap_sendpacket %s\n",pcap_geterr(pSendarp));
			return;
		}
		times++;

		nanosleep(&send_ts,NULL);
		printf("Send %d times arp packet\n",times);
	}
}


int main()
{


	int pth_arp = 0;

	pth_arp = pthread_create(&arpkeepsending, NULL, (void*)SendingArp, NULL);
	if(pth_arp != 0)
	{
		printf("Create thread error, exit program...................\n");
		exit(1);
	}

	pthread_join(arpkeepsending, NULL);

	return 0;
}
