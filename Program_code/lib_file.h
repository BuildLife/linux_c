/*Basic lib*/
#define _BSD_SOURCE
#define _USE_BSD 
#include <stdio.h>
#include <stdlib.h>

/*use in true & false type : boolean type*/
#include <stdbool.h>

/*time lib*/
#include <time.h>
#include <unistd.h>

/*Variable type*/
//#include <bsd/string.h>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>
#include <memory.h>

/*socket include*/
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>


#include <pcap.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/if_ether.h>


/*for kbhit*/
//#include <conio.h>
