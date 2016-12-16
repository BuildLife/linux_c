#include "lib_file.h"


#define BAUDRATE B38400
#define MAPDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1

#define FALSE 0
#define TRUE 1


volatile int STOP = FALSE;

void StopSignal()
{
	STOP = TRUE;	
}

/*void SignalAlarm(int sigo)
{
	
}
*/

int main(int argc,char *argv[])
{
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
		exit(-1);
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
		if(buf[0] == 'z') STOP=TRUE;
	}

	return 0;
}
