#include "lib_file.h"
 

#define BAUDRATE B38400
#define MAPDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1

#define FALSE 0
#define TRUE 1


void ThreadSocket();
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

int main(int argc,char *argv[])
{

	int pth_socserver = 0;
	//signal(SIGINT,&StopSignal);
	//signal(SIGALARM,);
	int fd, c, res, wes;
	struct termio options;
	char buf[255];
	char *w_buf = "configure terminal\n";
	char w_o_buf[64] = {0};
	char *vlan_mode = "default";

	vlan_mode = argv[1];
	memset(w_o_buf, 0, sizeof(w_o_buf));
	sprintf(w_o_buf,"vlan rule %s\n",vlan_mode);
	
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
	/*Enter in thread socket server*/
	pth_socserver = pthread_create(&pthreadSocketServerRunning, NULL, (void*)ThreadSocket, NULL);
	if(pth_socserver < 0)
	{
		printf("Create Socket server Error\n");
		exit(1);
	}
	pthread_join(pthreadSocketServerRunning, NULL);

	while(STOP == FALSE)
	{
		res = read(fd, buf, 255);
		sleep(1);
		buf[res] = 0;
		printf(":%s\n",buf);
		write(fd,w_o_buf,sizeof(w_o_buf));
		sleep(2);
		if(buf[0] == 'z') STOP=TRUE;
	}

	return 0;
}

void ThreadSocket()
{
	int sockfd;
	int clientfd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buffer[128] = {0x0a,0x03,0x88,0x99,0xff};


	struct in_addr LocalInterface;

	//create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		perror("Opening socket server error\n");
		exit(1);
	}
	else
		printf("Opening socket sever Success\n");

	//initiallze structure
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(9998);

	//for client
	server_addr.sin_addr.s_addr = INADDR_ANY;

	/*for multi socket client to connect*/
	/*LocalInterface.s_addr = inet_addr("127.0.0.1");
	if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, (char*)&LocalInterface,sizeof(LocalInterface)) < 0)
	{
		perror("Setting local interface error");
		exit(1);
	}	
	else
		printf("Setting local interface Success\n");
*/
	//assign a port number to socket
	bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	//listen to client to connect
	listen(sockfd, 20);

	while(1)
	{
		//wait and accept client to connection
		clientfd = accept(sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr));

		
		//sending to client message
		send(clientfd,buffer,sizeof(buffer),0);
		printf("Sending to client:%s\n",buffer);


	
		close(clientfd);
	}
	//close socket server
	close(sockfd);

}
