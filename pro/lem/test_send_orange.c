#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00

#define DEFAULT_IF	"eth0"
#define BUF_SIZ		1024

int main(int argc, char *argv[]){

	int sockfd;
    int tx_len = 0;
	char sendbuf[BUF_SIZ]={0xaa,0xaa,0xaa,0xaa,0xaa,0xaa,
                            0xbb,0xbb,0xbb,0xbb,0xbb,0xbb};


}