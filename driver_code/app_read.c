#include<stdio.h>
#include<linux/fs.h>
#include<fcntl.h>
#include<string.h> //for strlen & memset

int main(int argc,char **argv)
{

	int f;
	//FILE *f;
	int i = 0;
	int num = 0;
	f = open("/dev/hello_mo",O_RDWR);
	if(f == -1)
	{
		printf("open failed\n");
		return -1;
	}
	//ioctl(f,IOCTL_WRITE,num);
	//fd = fopen("/dev/hello_mo","r");
	char buf[1024] = {0};
	//fgets(buf,sizeof(buf),f);
	//printf("Write some thing :\n");
	//scanf("%s",buf);
	write(f,argv[1],strlen(argv[1]));
	char readbuf[128];
	memset(readbuf,0,128);
	while(read(f,&readbuf[i++],128));
	
	printf("%s\n",readbuf);
	close(f);
	return 0;
}
