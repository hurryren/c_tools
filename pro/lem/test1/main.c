#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <pthread.h>

#include "rawsocket.h"


const uint8_t MAC_ADDR[6] = {0x00,0x00,0x00,0x00,0x00,0x00}; // 源和目的mac地址都使用本地物理地址
struct sockaddr_ll sockaddrll1;
struct sockaddr_ll sockaddrll2;
int sockfd1,sockfd2;
uint8_t	rx[1600];
uint8_t	tx[1600];
uint8_t	rtx[1600];

int init();
static void get_mac_from_device(uint8_t mac[6],const char *devicename);
static void print_buf(unsigned char *buf, int len);
void recev();
void rx_to_tx();

uint8_t send_data[128] = {0x00,0x00,0x00,0x00,0x00,0x00,
                    0x00,0x00,0x00,0x00,0x00,0x00,
                    0x08,0x00,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x06,0x45,0x45, // 0x06, 0x11
                    0x11,0x22,0x33,0x44, // source ip
                    0x55,0x55,0x66,0x66, // dest ip
                    0x55,0x55,0x66,0x66, // dest ip
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x45,0x45,
                    0x45,0x45,0x00,0x00
                    };


int main(){

    init();
    memcpy(send_data,MAC_ADDR,6);
    memcpy(send_data+6,MAC_ADDR,6);

    pthread_t tid_rx,tid_rtx;
    pthread_create(&tid_rx,0,(void *)recev,NULL);
    pthread_create(&tid_rtx,0,(void *)rx_to_tx,NULL);

    memcpy(tx,send_data,64);

    while(1){
        switch (getchar())
        {
        case 'u': // udp
            tx[23] = 0x11;
            send_packet(sockfd1,tx,64,sockaddrll2);

            break;
        case 't': // tcp
            tx[23] = 0x06;
            send_packet(sockfd1,tx,64,sockaddrll2);

            break;
        case 'q':
            printf("exit\n");
            return 0;

        default:
            printf("please input u(udp) or t(tcp) or q(exit)\n");
            break;
        }
    }




    return 0;
}



int init(){
    memset(tx,0,1600);
    memset(rx,0,1600);
    memset(rtx,0,1600);
    sockfd1 = open_raw_socket("eno1",&sockaddrll1);
    sockfd2 = open_raw_socket("eno2",&sockaddrll2);
    if(sockfd1 < 0 || sockfd2 < 0){
        exit(1);
    }
    get_mac_from_device(MAC_ADDR,"eno1");
    print_buf(MAC_ADDR,6);


}

static void get_mac_from_device(uint8_t mac[6], const char *devicename)
{

	int	fd;
	int	err;
	struct ifreq	ifr;

	fd = socket(PF_PACKET, SOCK_RAW, htons(0x0806));
	assert(fd != -1);

	assert(strlen(devicename) < IFNAMSIZ);
	strncpy(ifr.ifr_name, devicename, IFNAMSIZ);
	ifr.ifr_addr.sa_family = AF_INET;

	err = ioctl(fd, SIOCGIFHWADDR, &ifr);
	assert(err != -1);
	memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);

	err = close(fd);
	assert(err != -1);
	return;
}

static void print_buf(unsigned char *buf, int len)
{
    int i;
    if (buf == NULL || len == 0)
        return;
    for (i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
        if (0 == (i + 1) % 16)
            printf("\n");
    }
    printf("\n");
}


void recev(){
    for(;;){
        if(get_next_packet(sockfd1, rx,MAC_ADDR,MAC_ADDR) == 1){
            printf("\neno1 receved a packet\n");
            print_buf(rx,sizeof(rx));
        }
    }
}
void rx_to_tx(){
    for(;;){
        if(get_next_packet(sockfd2, rtx,MAC_ADDR,MAC_ADDR) == 1){
            printf("\neno2 receved a packet\n");
            print_buf(rtx,sizeof(rtx));
            if(send_packet(sockfd2,rtx,sizeof(rtx),sockaddrll2) < 0){
                printf("\n rtx send error\n");
            }
        }
    }
}