#include "lib_file.h"


char location[] = "/dev/tty";
char MAPDEVICE[] = "USB0";
char *Option82 = "disable";
char vlan_mode[32] = {0};
char MainBuffer[32];
static int Runtimes = 0, Autotest = 0;
static int StartVID = 0;

/*Use in Start open socket command*/
static int OpenSocketFlag = 0;


/*Define use function*/
void ThreadSocket();
void ThreadCmcControl();
void MainMenu();
void SocketMenu();
void SetSendClientValue(int *);

/*Use in socket server*/
static int sockfd;
static int clientfd;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

volatile int STOP = FALSE;

void StopSignal()
{
	STOP = TRUE;	
}

/*Use in socket server running*/
pthread_t pthreadSocketServerRunning;

/*Use in connect Com Port*/
pthread_t pthreadComPort;

int main(int argc, char *argv[])
{
	int i = 0;
	int pth_socserver = 0, pth_usbport = 0;
	//signal(SIGINT,&StopSignal);
	//signal(SIGALARM,);
	printf("\n");
	if(argc != 2)
	{
		sprintf(location, "%s%s", location, MAPDEVICE);
	}
	else if(argc == 2)
	{
		strcpy(MAPDEVICE, argv[1]);
		sprintf(location, "%s%s", location, MAPDEVICE);
	}
	printf("Current Com port number is %s\n", location);
	printf("\n");

	MainMenu();

	while(fgets(MainBuffer, sizeof(MainBuffer), stdin) != NULL)
	{
		if(!strcmp(MainBuffer, "socket\n\0") || !strcmp(MainBuffer, "SOCKET\n\0"))
		{
			pth_socserver = pthread_create(&pthreadSocketServerRunning, NULL, (void*)ThreadSocket, NULL);
			if(pth_socserver < 0)
			{
				printf("Create Socket server Error\n");
				exit(1);
			}
			pthread_join(pthreadSocketServerRunning, NULL);
			
		}
		else if(!strcmp(MainBuffer, "h\n\0") || !strcmp(MainBuffer, "H\n\0"))
		{
			MainMenu();
		}
		// only for testing 
		else if(!strcmp(MainBuffer, "usb\n\0") || !strcmp(MainBuffer, "USB\n\0"))
		{
			ThreadCmcControl();
		}
		else if(!strcmp(MainBuffer, "exit\n\0") || !strcmp(MainBuffer, "EXIT\n\0"))
		{
			exit(0);
		}
	}

	close(clientfd);
	close(sockfd);
	return 0;
}

void ThreadCmcControl()
{
	int fd, c, res, wes;
	struct termios options,oldtio; //Notice that struct "termios" NOT termio 
	char buf[255];
	char *w_buf = "configure terminal\n";
	char w_o_buf[64] = {0};
	char Option82_w_buf[64] = {0};

	//clear w_o_buf to zero
	memset(w_o_buf, 0, sizeof(w_o_buf));
	memset(Option82_w_buf, 0, sizeof(Option82_w_buf));

	fd = open(location, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( fd < 0 )
	{
		perror(location);
	}
	
	fcntl(fd, F_SETFL, FNDELAY);

	//set USB Comport default values
	tcgetattr(fd, &oldtio);

	//Get the original setting
	tcgetattr(fd, &options);
	bzero(&options, sizeof(options));
	// Setting the Baudrate
	cfsetispeed(&options,BAUDRATE);
	cfsetospeed(&options,BAUDRATE);
	
	//enable the receiver and set local mode
	options.c_cflag |= (CLOCAL|CREAD);

	//setting the character size (8-bits)
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	// Setting Parity Checking : NONE (NO Parity)
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;

	//Clear the HUPCL bit
	//options.c_cflag &= ~HUPCL;

	// Disable Hardware flow control
	options.c_cflag &= ~CRTSCTS;

	// Disable Input Parity Checking
	options.c_iflag &= ~INPCK;
	//options.c_iflag |= (INPCK | ISTRIP);

	// Disable software flow control
	options.c_iflag &= ~(IXON|IXOFF|IXANY);

	//options.c_iflag &= ~(IGNPAR|ICRNL);

	// Output raw data
	options.c_oflag &= ~OPOST;
	//Enable output raw data
	//options.c_oflag |= OPOST | ONLCR;

	// Disablestd input 
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);
	//options.c_lflag &= ~(ICANON|ECHO|ISIG);

	// clean the current setting
	tcflush(fd, TCIFLUSH);

	// Enable the new setting right now
	tcsetattr(fd, TCSANOW, &options);
	
	char cmd_buf;
	if(!strcmp(MainBuffer, "USB\n\0") || !strcmp(MainBuffer, "usb\n\0"))
	{
		printf("test for enter mode(DVGM/SVGM):\n");
		scanf("%s", &vlan_mode);
		printf("test for option82(Y/N)[N]:\n");
		scanf("%c");
		cmd_buf = getchar();
		if(cmd_buf == 'Y' || cmd_buf == 'y')
			Option82 = "enable";
		else if(cmd_buf == 'N' || cmd_buf == 'n')
			Option82 = "disable";
	}
	
	sleep(1);
	write(fd, "root\n", 5);

	sleep(1);
	if(!strcmp(vlan_mode, "DVGM"))
		sprintf(w_o_buf, "vlan rule disable\n");
	else if(!strcmp(vlan_mode, "SVGM"))
		sprintf(w_o_buf, "vlan rule enable\n");
	
	if(!strcmp(Option82, "enable"))
		sprintf(Option82_w_buf, "option82 enable\n");
	else if(!strcmp(Option82, "disable"))
		sprintf(Option82_w_buf, "option82 disable\n");


	int runtimes = 0;
	
	while(STOP == FALSE)
	{
		res = read(fd, buf, 255);
		sleep(1);
		buf[res] = '\0';
		printf("%s\n", buf);
		
		if(strstr(buf, "login: "))
		{
			write(fd, "root\n", 5);
			sleep(1);
		}

		if(strstr(buf, "Password:"))
		{
			write(fd, "admin\n", 6);
			sleep(1);
		}

		if(strstr(buf, "Controller#"))
		{
			write(fd, "configure terminal\n", 20);
			sleep(1);
		}
		if(strstr(buf, "Controller-cfg#"))
		{
			write(fd, w_o_buf, sizeof(w_o_buf));
			sleep(1);
			write(fd, Option82_w_buf, sizeof(Option82_w_buf));
		}
		sleep(2);
		runtimes++;
		if(runtimes > 3)
		{
			printf("Enter in %s mode\n", vlan_mode);
			printf("set mode ok\n");
			STOP = TRUE;
		}
	}

	if(fd)
	{
		printf("Close the USB Com Port\n");
		close(fd);
	}
	
	//set USB Comport to default values
	tcgetattr(fd, &oldtio);

	//let USB Comport can running while loop
	STOP = FALSE;
}


void ThreadSocket()
{
	int pth_usbport = 0;
	int send_res = 0;
	int cl_addrlen = sizeof(client_addr);
	int se_addrlen = sizeof(struct sockaddr_in);
	char buffer[32] = {0};
	char recvbuffer[32] = {0};
	
	fd_set rfds;

	int yes = 1;

	//create socket
	/*tcp socket*/
	//sockfd = socket(PF_INET, SOCK_STREAM, 0);
	
	/*udp socket*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{
		perror("Opening socket server error\n");
		exit(1);
	}
	else
	{
		printf("Opening socket sever Success\n");
	}
	//initiallze structure
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9998);

	//for client
	server_addr.sin_addr.s_addr = INADDR_ANY;

	/*for multi socket client to connect
	  Avoid that address already in use,and can be use the same port.*/
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) < 0)
	{
		perror("Setting local interface error");
		exit(1);
	}	
	else
		printf("Setting local interface Success\n");

	//assign a port number to socket
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	/*tcp socket*/
	//listen to client to connect
	//listen(sockfd, 20);

	int c = 0;
	int GetMode = 0;
	char Cmdbuf[64];
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);
		FD_SET(clientfd, &rfds);

		//wait and accept client to connection
		printf("Waiting for client to connect........................\n");
		
		if(select(sockfd+1, &rfds, NULL, NULL, NULL)) {
			if(FD_ISSET(sockfd, &rfds)) {
		/*udp socket*/
		clientfd = recvfrom(sockfd, recvbuffer, sizeof(recvbuffer)-1, 0, (struct sockaddr*)&client_addr, &cl_addrlen);
		
		/*ASCII -> C  L  I  E  N  T  O  P  E  N
				   43 4C 49 45 4E 54 4F 50 45 4E */
		if(recvbuffer[0] == 0x43 && recvbuffer[1] == 0x4C && recvbuffer[2] == 0x49 && recvbuffer[3] == 0x45 && recvbuffer[4] == 0x4E && recvbuffer[5] == 0x54 && recvbuffer[6] == 0x4F && recvbuffer[7] == 0x50 && recvbuffer[8] == 0x45 && recvbuffer[9] == 0x4E)
		{
			/*Tell client already connect to server
			ACSII -> S  E  R  V  E  R  O  P  E  N
					53 45 52  56 45 52 4F 50 45 4E*/
			recvbuffer[0] = 0x53;
			recvbuffer[1] = 0x45;
			recvbuffer[2] = 0x52;
			recvbuffer[3] = 0x56;
			recvbuffer[4] = 0x45;
			recvbuffer[5] = 0x52;
	
			/*udp socket*/
			sendto(sockfd, recvbuffer, sizeof(recvbuffer)-1, 0, (struct sockaddr*)&client_addr, cl_addrlen);

			memset(recvbuffer, 0, sizeof(recvbuffer));
			//SetSendClientValue(&buffer[0]);

			SocketMenu();
		while(fgets(Cmdbuf, sizeof(Cmdbuf), stdin)!=NULL)
		{
			if(!strcmp(Cmdbuf,"auto\n\0") || !strcmp(Cmdbuf, "AUTO\n\0"))
			{
				SetSendClientValue(&buffer[0]);
				sleep(2);
				/*Enter in thread USB COMPORT*/
				printf("Connect Controller and start to set %s mode\n", vlan_mode);
				ThreadCmcControl();

				sleep(2);
				buffer[1] = (Runtimes >> 8) & 0xff;
				buffer[2] = (Runtimes) & 0xff;
				buffer[3] = (StartVID >> 8) & 0xff;
				buffer[4] = (StartVID) & 0xff;
				if(Option82 == "enable")
					buffer[5] = 0x01;
				else if(Option82 == "disable")
					buffer[5] = 0x02;

				//sending to client message
				
				/*udp socket*/
				if(sendto(sockfd, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&client_addr, cl_addrlen) > 0)
				{
					printf("Sending to client Success, Start to test\n");
				}
				else
					printf("Sending to client failed................\n");

				/*udp socket*/
			//	recvfrom(sockfd,recvbuffer,sizeof(recvbuffer)-1,0,(struct sockaddr*)&client_addr,&cl_addrlen);
			}
			else if(!strcmp(Cmdbuf, "h\n\0") || !strcmp(Cmdbuf, "H\n\0"))
			{
				SocketMenu();
			}
			/*else if(!strcmp(Cmdbuf, "stop\n\0"))
			{
				recvfrom(sockfd, recvbuffer, sizeof(recvbuffer)-1, 0, (struct sockaddr*)&client_addr, &cl_addrlen);
				if(recvbuffer[0] == 0x53 && recvbuffer[1] == 0x54 && recvbuffer[2] == 0x4F && recvbuffer[3] == 0x50)
				{
					printf("Client have been STOP\n");
				}
				recvbuffer[0] = 0x53;
				recvbuffer[1] = 0x54;
				recvbuffer[2] = 0x4F;
				recvbuffer[3] = 0x50;
				//sendto(clientfd,recvbuffer,4,0,(struct sockaddr*)&client_addr,&cl_addrlen);
			}*/
			else if(!strcmp(Cmdbuf, "exit\n\0") || !strcmp(Cmdbuf, "EXIT\n\0"))
			{
				int i = 0;
				int j = 0;
				for( ; j < 6; j++)
				{
					buffer[j] = 0x00;
				}
				printf("Socket thread cancel.............. \n");
				
				MainMenu();
				
				buffer[6] = 0x43;
				buffer[7] = 0x4C;
				buffer[8] = 0x4F;
				buffer[9] = 0x53;
				buffer[10] = 0x45;
				
				/*udp socket*/
				sendto(sockfd, buffer, 11, 0, (struct sockaddr*)&client_addr, cl_addrlen);
				
				close(clientfd);
				close(sockfd);
				pthread_cancel(pthreadSocketServerRunning);
				
				//Clear buffer to zero
				memset(buffer, 0, sizeof(buffer));
				
			}

		} //while fgets
		} //if client open command
		close(clientfd);
		FD_CLR(clientfd, &rfds);
		}//FD_ISSET
		} //select()
	}// while(1)
	close(sockfd);
	FD_CLR(sockfd, &rfds);
}

//Show Main User Menu to use
void MainMenu()
{
	printf("*********** Main User Menu **********\n");
	printf("Opening Socket Server for Ready to auto test: socket or SOCKET\n");
	printf("Safe leave this Main program : exit or EXIT\n");
	printf("Show Main Menu : h or H\n");
	printf("*************************************\n");
}

//Show Socket Menu to use
void SocketMenu()
{
	printf("*********** Socket User Menu **********\n");
	printf("Sending auto test command : auto or AUTO\n");
	printf("Leave the Socket Serve thread Function : exit or EXIT\n");
	printf("Show Socket Menu : h or H\n");
	printf("***************************************\n");
}


void SetSendClientValue(int *mode)
{
	char Option82_cmd;

	printf("Set Value Sending to client for auto test:\n");
	printf("Set Option82 (Y/N) [N]\n");
	scanf("%c",&Option82_cmd);
	if(Option82_cmd == 'Y' || Option82_cmd == 'y')
		Option82 = "enable";
	else if(Option82_cmd == 'N' || Option82_cmd == 'n')
		Option82 = "disable";
	else
		Option82 = "disable";

	printf("Enter Test Mode (DVGM/SVGM): \n");
	
	do{
		if(scanf("%s", &vlan_mode) != 1)
		{
			printf("Please at least enter a mode(DVGM/SVGM)\n");
		}
		else if(strcmp(vlan_mode, "DVGM") != 0 && strcmp(vlan_mode, "SVGM") != 0)
		{
			printf("Please Enter the right word(DVGM/SVGM)\n");
		}

	}while(strcmp(vlan_mode, "DVGM") != 0 && strcmp(vlan_mode, "SVGM") != 0);

	if(!strcmp(vlan_mode, "DVGM"))
		*mode = 1;
	else if(!strcmp(vlan_mode, "SVGM"))
		*mode = 2;

	printf("Enter Running Times or 0 Keeping the loop running: \n");
	scanf("%d", &Runtimes);

	printf("Enter Start VID(2049 ~ 2512) : \n");
	if(scanf("%d", &StartVID) != 1)
	{	
		printf("Enter the wrong, the VID have begin 2049\n");
		StartVID = 2049;
	}
	else if(StartVID < 2049 || StartVID > 2512)
	{
		printf("Enter the VID out of the range, so the VID start on 2049\n");
		StartVID = 2049;
	}
}
