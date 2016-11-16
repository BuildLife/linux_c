#include "lib_file.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 50

pthread_t pthreadServer;
pthread_t pthreadClient;
void socket_server()
{
	
	printf("Start to socket server\n");
	int sockfd;
	struct sockaddr_in dest;
	char buffer[1024];
	char buffer1[2048];

	/*create socket*/
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("create socket server error\n");
		return -1;
	}


	/*initialize structure dest*/
	bzero(&dest,sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(9998);

	/*this line is different from client*/
	//dest.sin_addr.s_addr = INADDR_ANY;
	dest.sin_addr.s_addr = inet_addr("192.168.5.5");
	/*Assign a port number to socket*/
	bind(sockfd, (struct sockaddr*)&dest,sizeof(dest));

	/*make it listen to socket with max 20 connections*/
	listen(sockfd,20);

		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);
		clientfd = accept(sockfd,(struct sockaddr*)&client_addr, &addrlen);
	/*loop -- accepting connection from client forever*/
	while(1)
	{
	//	int clientfd;
	//	struct sockaddr_in client_addr;
	//	int addrlen = sizeof(client_addr);

		printf("Server Enter message:");
		scanf("%s",buffer);
		//pthread_create(,,NULL);
		/*Wait and Accept connection*/
//		clientfd = accept(sockfd,(struct sockaddr*)&client_addr, &addrlen);

		/*Send message*/
	 if(send(clientfd,buffer,sizeof(buffer),0)<0)
	 {
			printf("send data failed\n");

	 }
		int res = recv(clientfd,buffer1,sizeof(buffer1),0);
		printf("receive from client: %s, %d bytes\n",buffer1,res);

		/*close(client)*/
		close(clientfd);
	}
	
	/*close(server)*/
	close(sockfd);

}
void socket_client()
{
	printf("Socket client start ---------------\n");
	int sockdest;
	struct sockaddr_in serv_addr;
	char S_buff[MAX_SIZE],R_buff[MAX_SIZE];
	char message[1024], server_reply[2048];
	if((sockdest = socket(AF_INET,SOCK_STREAM,0))<0)
		printf("Failed creating socket\n");

		bzero((unsigned char *)&serv_addr,sizeof(serv_addr));
	
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = inet_addr("192.168.5.5");
		serv_addr.sin_port = htons(9998);

		if(connect(sockdest,(struct sockaddr *)&serv_addr,sizeof(serv_addr)))
		{
			printf("Failed to connect to server\n");
			return -1;
		}
			printf("Socket Connect Success\n");
			while(1)
			{
				printf("Client Enter message:");
				scanf("%s",message);

				if(send(sockdest,message,sizeof(message),0)<0)
				{
					puts("send failed");
					return 1;
				}
				if(recv(sockdest,server_reply,2048,0)<0)
				{
					puts("recv failed");
					break;
				}
				printf("Receive Server data : %s\n",server_reply);
			}
			close(sockdest);

}

int main()
{
	
	int re_c = pthread_create(&pthreadClient,NULL,(void*)socket_client,NULL);
	if(re_c != 0)
	{
			printf("create pthread client error\n");
			exit(1);
	}
	int re_s = pthread_create(&pthreadServer,NULL,(void*)socket_server,NULL);
	if(re_s != 0)
	{
			printf("create pthread server error\n");
			exit(1);
	}
	else
		printf("create pthread server success\n");

	pthread_join(pthreadClient,NULL);
	pthread_join(pthreadServer,NULL);

	return 0;
}
