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

const uint8_t MAC_ADDR[6] = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66};   // 源和目的mac地址都使用本地物理地址
const uint8_t MAC_ADDR_P[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; // 源和目的mac地址都使用本地物理地址
struct sockaddr_ll sockaddrll1;
struct sockaddr_ll sockaddrll2;
int sockfd1, sockfd2;
uint8_t rx[1600];
uint8_t tx[1600];
uint8_t rtx[1600];

int init();
static void get_mac_from_device(uint8_t mac[6], const char *devicename);
static void print_buf(unsigned char *buf, int len);
void recev();
void rx_to_tx();

uint8_t tcp_header[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0x00,
    0x45, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x06, 0xff, 0xff, // 0x06, 0x11
    0x02, 0x02, 0x02, 0x02, // source ip
    0x02, 0x02, 0x02, 0x02, // dest ip
    0x03, 0x03, 0x03, 0x03, // port
    0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04
};

uint8_t udp_header[64] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0x00,
    0x45, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x11, 0xff, 0xff, // 0x06, 0x11
    0x02, 0x02, 0x02, 0x02, // source ip
    0x02, 0x02, 0x02, 0x02, // dest ip
    0x03, 0x03, 0x03, 0x03, // port
    0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04,
    0x04, 0x04
};

const int packet_len[] = {64,1514,100,256,500,512,1000,1024};
#define NUM 8
int main()
{
    char c[10];
    int buflen = 64;

    // 打开网卡设备
    init();

    memcpy(udp_header, MAC_ADDR, 6);
    memcpy(udp_header + 6, MAC_ADDR, 6);
    memcpy(tcp_header, MAC_ADDR, 6);
    memcpy(tcp_header + 6, MAC_ADDR, 6);

    // eno2转发线程和eno1接收线程
    pthread_t tid_rx, tid_rtx;
    pthread_create(&tid_rx, 0, (void *)recev, NULL);
    pthread_create(&tid_rtx, 0, (void *)rx_to_tx, NULL);

    printf("please input c(continue) or q(exit)\n");
    for(int i = 0; i< NUM ;i++){
        buflen = packet_len[i];

        for(int j = 0;j<2;j++){
            memset(tx,0x00,1600);
            fgets(c, 8, stdin);
            switch (c[0])
            {
                case 'c':
                    if(j = 0){ // tcp
                        printf("\n\neno1 send tcp packet\n");
                        memcpy(tx, tcp_header,64);
                    }else{
                        printf("\n\neno1 send udp packet\n");
                        memcpy(tx, udp_header,64);
                    }
                    memset(tx+64,0x04,buflen-64);
                    send_packet(sockfd1, tx, buflen, sockaddrll1);
                    break;
                case 'q':
                    printf("exit\n");
                    return 0;
                default:
                    printf("please input 'c' to continue or q(exit)\n");
                    break;
            }
        }
    }


    // return 0;
}

int init()
{
    memset(tx, 0, 1600);
    memset(rx, 0, 1600);
    memset(rtx, 0, 1600);
    sockfd1 = open_raw_socket("eno1", &sockaddrll1);
    sockfd2 = open_raw_socket("eno2", &sockaddrll2);
    if (sockfd1 < 0 || sockfd2 < 0)
    {
        printf("打开网络设备失败\n");
        exit(1);
    }
    // get_mac_from_device(MAC_ADDR,"eno1");
    // print_buf(MAC_ADDR, 6);
    printf("成功打开网络设备\n");
}

static void get_mac_from_device(uint8_t mac[6], const char *devicename)
{

    int fd;
    int err;
    struct ifreq ifr;

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

void recev()
{
    for (;;)
    {
        if (get_next_packet(sockfd1, rx, MAC_ADDR_P, MAC_ADDR) == 1)
        {
            printf("\neno1 receved a packet\n");
            print_buf(rx, sizeof(rx));
        }
    }
}
void rx_to_tx()
{
    for (;;)
    {
        if (get_next_packet(sockfd2, rtx, MAC_ADDR, MAC_ADDR) == 1)
        {
            printf("\neno2 receved a packet\n");
            print_buf(rtx, sizeof(rtx));
            memcpy(rtx, MAC_ADDR_P, 6);
            send_packet(sockfd2, rtx, sizeof(rtx), sockaddrll2);
        }
    }
}
