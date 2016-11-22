/*Basic lib*/
#define _BSD_SOURCE
//#define __BEGIN_DECLS
#define __FAVOR_BSD 
#define _USE_BSD 
//#define _LINUX_UDP_H
#include <stdio.h>
#include <stdlib.h>

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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <netdb.h>
#include <errno.h>


#include <pcap.h>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/if_ether.h>
