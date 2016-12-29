#include "lib_file.h"
 

#define BAUDRATE B38400
#define MAPDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1

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

	/*if(argc == 4)
	{
		vlan_mode = argv[1];
		Runtimes = atoi(argv[2]);
		Autotest = atoi(argv[3]);
	}*/

	/*Enter in thread USB COMPORT*/
	/*pth_usbport = pthread_create(&pthreadComPort, NULL, (void*)ThreadCmcControl, NULL);
	if(pth_usbport < 0)
	{
		printf("Create Socket server Error\n");
		exit(1);
	}*/

	/*Enter in thread socket server*/
	/*pth_socserver = pthread_create(&pthreadSocketServerRunning, NULL, (void*)ThreadSocket, NULL);
	if(pth_socserver < 0)
	{
		printf("Create Socket server Error\n");
		exit(1);
	}*/

	//pthread_join(pthreadComPort, NULL);
	//pthread_join(pthreadSocketServerRunning, NULL);

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
	struct termio options;
	char buf[255];
	char *w_buf = "configure terminal\n";
	char w_o_buf[255] = {0};

	memset(w_o_buf, 0, sizeof(w_o_buf));

	if(!strcmp(vlan_mode,"DVGM"))
		sprintf(w_o_buf,"vlan rule disable\n");
	else if(!strcmp(vlan_mode,"SVGM"))
		sprintf(w_o_buf,"vlan rule enable\n");
	

	fd = open(MAPDEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if( fd < 0 )
	{
		perror(MAPDEVICE);
		//exit(-1);
	}
	
	fcntl(fd, F_SETFL, FNDELAY);

	//Get the original setting
	tcgetattr(fd,&options);

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

	// Disable Hardware flow control
	options.c_cflag &= ~CRTSCTS;

	// Disable Input Parity Checking
	options.c_iflag &= ~INPCK;

	// Disable software flow control
	options.c_iflag &= ~(IXON|IXOFF|IXANY);

	options.c_iflag &= ~(IGNPAR|ICRNL);

	// Output raw data
	options.c_oflag &= ~OPOST;

	// Disablestd input 
	options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);

	// clean the current setting
	tcflush(fd, TCIFLUSH);

	// Enable the new setting right now
	tcsetattr(fd, TCSANOW, &options);

	write(fd, "\n",1);
	sleep(1);

	while(STOP == FALSE)
	{
		res = read(fd, buf, 255);
		sleep(1);
		buf[res] = 0;
		printf(":%s\n",buf);
		write(fd,w_o_buf,sizeof(w_o_buf));
		sleep(2);
	}

}


void ThreadSocket()
{
	int send_res = 0;
	int cl_addrlen = sizeof(client_addr);
	char buffer[128] = {0};
	
	int yes = 1;

	//create socket
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
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

	//listen to client to connect
	listen(sockfd, 20);

	int c = 0;
	int GetMode = 0;
	char Cmdbuf[64];
	while(1)
	{
		//wait and accept client to connection
		printf("Waiting for client to connect........................\n");
		clientfd = accept(sockfd,(struct sockaddr*)&client_addr,&cl_addrlen);
		if(clientfd < 0)
		{
			printf("Client Connect Fail, Please Check it again.......\n");
			pthread_cancel(pthreadSocketServerRunning);
			printf("Leave the Socket Thread Function, Please Re-start the Socket Server.\n");
		}
		else
		{
			printf("Client Connect Success...........................\n");
			SetSendClientValue(&buffer[0]);
		}

		SocketMenu();
		while(fgets(Cmdbuf, sizeof(Cmdbuf), stdin)!=NULL)
		{
			if(!strcmp(Cmdbuf,"auto\n\0"))
			{
				buffer[1] = (Runtimes >> 8) & 0xff;
				buffer[2] = (Runtimes) & 0xff;
				buffer[3] = (StartVID >> 8) & 0xff;
				buffer[4] = (StartVID) & 0xff;
				buffer[5] = 0x01;

				//sending to client message
				if(send(clientfd,buffer,sizeof(buffer),0) < 0)
				{
					printf("Cannot send out to client\n");
				}
				recv(clientfd, buffer, sizeof(buffer),0);
			}
			else if(!strcmp(Cmdbuf, "h\n\0"))
			{
				SocketMenu();
			}
			else if(!strcmp(Cmdbuf, "stop\n\0"))
			{
				buffer[5] = 2;
				send(clientfd,buffer,sizeof(buffer),0);
			}
			else if(!strcmp(Cmdbuf, "exit\n\0"))
			{
				printf("Socket thread cancel.............. \n");
				close(clientfd);
				close(sockfd);
				pthread_cancel(pthreadSocketServerRunning);
				MainMenu();
			}
			else
				printf("No this optins\n");
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
	scanf("%s",&vlan_mode);
	if(!strcmp(vlan_mode,"DVGM"))
		*mode = 1;
	else if(!strcmp(vlan_mode,"SVGM"))
		*mode = 2;

	printf("Enter Running Times or 0 Keeping the loop running: \n");
	scanf("%d",&Runtimes);

	printf("Enter Start VID : \n");
	if(scanf("%d",&StartVID) != 1)
	{	
		printf("Enter the wrong\n");
	}
}
