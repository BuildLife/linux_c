#include "lib_file.h"
 

#define BAUDRATE B38400
#define MAPDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /*Match with POSIX system*/

#define FALSE 0
#define TRUE 1


//char *vlan_mode = "default";
char vlan_mode[32] = {0};
int Runtimes = 0, Autotest = 0;
int StartVID = 0;

/*Use in Start open socket command*/
int OpenSocketFlag = 0;


/*Define use function*/
void ThreadSocket();
void ThreadCmcControl();
void MainMenu();
void SocketMenu();
void SetSendClientValue(int *);

/*Use in socket server*/
int sockfd;
int clientfd;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;

volatile int STOP = FALSE;

void StopSignal()
{
	STOP = TRUE;	
}

/*void SignalAlarm(int sigo)
{
	
}
*/

/*Use in socket server running*/
pthread_t pthreadSocketServerRunning;

/*Use in connect Com Port*/
pthread_t pthreadComPort;

int main(int argc,char *argv[])
{

	int pth_socserver = 0, pth_usbport = 0;
	//signal(SIGINT,&StopSignal);
	//signal(SIGALARM,);

	char MainBuffer[32];

	MainMenu();

	while(fgets(MainBuffer, sizeof(MainBuffer), stdin) != NULL)
	{
		if(!strcmp(MainBuffer, "socket\n\0"))
		{
			pth_socserver = pthread_create(&pthreadSocketServerRunning, NULL, (void*)ThreadSocket, NULL);
			if(pth_socserver < 0)
			{
				printf("Create Socket server Error\n");
				exit(1);
			}
			pthread_join(pthreadSocketServerRunning, NULL);
			
		}
		else if(!strcmp(MainBuffer, "h\n\0"))
		{
			MainMenu();
		}
		// only for testing 
		else if(!strcmp(MainBuffer, "usb\n\0") || !strcmp(MainBuffer, "USB\n\0"))
		{
			ThreadCmcControl();
		}
		else if(!strcmp(MainBuffer, "exit\n\0"))
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
	char w_o_buf[255] = {0};

	//clear w_o_buf to zero
	memset(w_o_buf, 0, sizeof(w_o_buf));

	fd = open(MAPDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( fd < 0 )
	{
		perror(MAPDEVICE);
	}
	
	fcntl(fd, F_SETFL, FNDELAY);

	//set USB Comport default values
	tcgetattr(fd,&oldtio);

	//Get the original setting
	tcgetattr(fd,&options);
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
	
	//sprintf(vlan_mode,"DVGM");
	sleep(1);
	//write(fd, " \n",2);
	write(fd,"root\n",5);

	sleep(1);
	if(!strcmp(vlan_mode,"DVGM"))
		sprintf(w_o_buf,"vlan rule disable\n");
	else if(!strcmp(vlan_mode,"SVGM"))
		sprintf(w_o_buf,"vlan rule enable\n");
	
	int runtimes = 0;
	
	while(STOP == FALSE)
	{
		res = read(fd, buf, 255);
		sleep(1);
		buf[res] = '\0';
		printf("%s\n",buf);
		
		if(strstr(buf,"login: "))
		{
			write(fd,"root\n",5);
			sleep(1);
		}

		if(strstr(buf,"Password:"))
		{
			write(fd,"admin\n",6);
			sleep(1);
		}

		if(strstr(buf,"Controller#"))
		{
			write(fd,"configure terminal\n",20);
			sleep(1);
		}
		if(strstr(buf,"Controller-cfg#"))
		{
			write(fd,w_o_buf,sizeof(w_o_buf));
		}
		sleep(2);
		runtimes++;
		if(runtimes > 4)
		{
			printf("Enter in %s mode\n",vlan_mode);
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
	tcgetattr(fd,&oldtio);

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
		//SocketMenu();
	}
	//initiallze structure
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9998);

	//for client
	server_addr.sin_addr.s_addr = INADDR_ANY;

	/*for multi socket client to connect
	  Avoid that address already in use,and can be use the same port.*/
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes,sizeof(yes)) < 0)
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
		//wait and accept client to connection
		printf("Waiting for client to connect........................\n");
		
		/*tcp socket*/
		//clientfd = accept(sockfd,(struct sockaddr*)&client_addr,&cl_addrlen);
		
		/*udp socket*/
		clientfd = recvfrom(sockfd,recvbuffer,sizeof(recvbuffer)-1,0,(struct sockaddr*)&client_addr,&cl_addrlen);
		/*if(clientfd < 0)
		{
			printf("Client Connect Fail, Please Check it again.......\n");
			pthread_cancel(pthreadSocketServerRunning);
			printf("Leave the Socket Thread Function, Please Re-start the Socket Server.\n");
		}
		else
		{
			printf("Client Connect Success...........................\n");
			SetSendClientValue(&buffer[0]);
		}*/
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
			sendto(sockfd,recvbuffer,sizeof(recvbuffer)-1,0,(struct sockaddr*)&client_addr,cl_addrlen);

			memset(recvbuffer,0,sizeof(recvbuffer));
			//SetSendClientValue(&buffer[0]);

			SocketMenu();
		while(fgets(Cmdbuf, sizeof(Cmdbuf), stdin)!=NULL)
		{
			if(!strcmp(Cmdbuf,"auto\n\0"))
			{
				SetSendClientValue(&buffer[0]);
				sleep(2);
				/*Enter in thread USB COMPORT*/
				ThreadCmcControl();
				/*pth_usbport = pthread_create(&pthreadComPort, NULL, (void*)ThreadCmcControl, NULL);
				if(pth_usbport < 0)
				{
					printf("Create USB Control server Error\n");
					exit(1);
				}*/
	//			pthread_join(pthreadComPort,NULL);

				sleep(2);
				buffer[1] = (Runtimes >> 8) & 0xff;
				buffer[2] = (Runtimes) & 0xff;
				buffer[3] = (StartVID >> 8) & 0xff;
				buffer[4] = (StartVID) & 0xff;
				buffer[5] = 0x01;

				//sending to client message
				/*tcp socket*/
				//if(send(clientfd,buffer,sizeof(buffer),0) < 0)
				/*if(sendto(sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&server_addr,se_addrlen) < 1)
				{
					//printf("Cannot send out to client\n");
					perror("sendto");
					return 1;
				}*/
				/*udp socket*/
				sendto(sockfd,buffer,sizeof(buffer)-1,0,(struct sockaddr*)&client_addr,cl_addrlen);

				/*tcp socket*/
				//if(recv(clientfd, buffer, sizeof(buffer),0) > 0)

				/*udp socket*/
				if(recvfrom(sockfd,recvbuffer,sizeof(recvbuffer)-1,0,(struct sockaddr*)&client_addr,&cl_addrlen) > 1)
				{
					printf("Socket Client was stop the sending loop\n");
				}
				
			}
			else if(!strcmp(Cmdbuf, "h\n\0"))
			{
				SocketMenu();
			}
			else if(!strcmp(Cmdbuf, "stop\n\0"))
			{
				recvbuffer[0] = 0x53;
				recvbuffer[1] = 0x54;
				recvbuffer[2] = 0x4F;
				recvbuffer[3] = 0x50;
				sendto(clientfd,recvbuffer,4,0,(struct sockaddr*)&client_addr,&cl_addrlen);
			}
			else if(!strcmp(Cmdbuf, "exit\n\0"))
			{
				int i = 0;
				int j = 0;
				for(;j<6;j++)
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
				
				/*tcp socket*/
				//send(clientfd,buffer,11,0);
				
				/*udp socket*/
				sendto(sockfd,buffer,11,0,(struct sockaddr*)&client_addr,cl_addrlen);
				
				close(clientfd);
				close(sockfd);
				pthread_cancel(pthreadSocketServerRunning);
				
				//Clear buffer to zero
				memset(buffer,0,sizeof(buffer));
				
			}
			else
				printf("No this optins\n");
		}
		}
		close(clientfd);
	}
	close(sockfd);
}

//Show Main User Menu to use
void MainMenu()
{
	printf("*********** Main User Menu **********\n");
	printf("Opening Socket Server for Ready to auto test: socket\n");
	printf("Safe leave this Main program : exit\n");
	printf("Show Main Menu : h\n");
	printf("*************************************\n");
}

//Show Socket Menu to use
void SocketMenu()
{
	printf("*********** Socket User Menu **********\n");
	printf("Sending auto test command : auto\n");
	printf("Leave the Socket Serve thread Function : exit\n");
	printf("Show Socket Menu : h\n");
	printf("***************************************\n");
}

void SetSendClientValue(int *mode)
{
	printf("Set Value Sending to client for auto test:\n");
	printf("Enter Test Mode (DVGM/SVGM): \n");
	
	do{
		if(scanf("%s",&vlan_mode) != 1)
		{
			printf("Please at least enter a mode(DVGM/SVGM)\n");
		}
		else if(strcmp(vlan_mode,"DVGM") != 0 && strcmp(vlan_mode,"SVGM") != 0)
		{
			printf("Please Enter the right word(DVGM/SVGM)\n");
		}

	}while(strcmp(vlan_mode,"DVGM") != 0 && strcmp(vlan_mode,"SVGM") != 0);

	if(!strcmp(vlan_mode,"DVGM"))
		*mode = 1;
	else if(!strcmp(vlan_mode,"SVGM"))
		*mode = 2;

	printf("Enter Running Times or 0 Keeping the loop running: \n");
	scanf("%d",&Runtimes);

	printf("Enter Start VID(2049 ~ 2512) : \n");
	if(scanf("%d",&StartVID) != 1)
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
