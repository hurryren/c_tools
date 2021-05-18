#include<stdio.h>
#include<stdint.h>
#include<string.h>


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

const uint8_t MAC_ADDR[6] = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66};   // 源和目的mac地址都使用本地物理地址
const uint8_t MAC_ADDR_P[6] = {0x99, 0x99, 0x99, 0x99, 0x99, 0x99}; // 源和目的mac地址都使用本地物理地址

uint8_t rx[1600];
uint8_t tx[1600];
uint8_t rtx[1600];

const int packet_len[] = {64,1514,100,256,500,512,1000,1024};
#define NUM 8
int buflen = 64;
int main(int argc, char* argv[]){
    char c[10];

    if(argc > 1)
        buflen  = atoi(argv[1]);
        printf("buflen = %d\n",buflen);


    // memcpy(udp_header, MAC_ADDR, 6);
    // memcpy(udp_header + 6, MAC_ADDR, 6);
    // memcpy(tcp_header, MAC_ADDR, 6);
    // memcpy(tcp_header + 6, MAC_ADDR, 6);

    // printf("please input c(continue) or q(exit)\n");
    // for(int i = 0; i< NUM ;i++){
    //     buflen = packet_len[i];

    //     for(int j = 0;j<2;j++){
    //         memset(tx,0x00,1600);
    //         fgets(c, 8, stdin);
    //         switch (c[0])
    //         {
    //             case 'c':
    //                 if(j == 0){ // tcp
    //                     printf("\n\neno1 send tcp packet,length = %d\n",buflen);
    //                     memcpy(tx, tcp_header,64);
    //                 }else{
    //                     printf("\n\neno1 send udp packet,length = %d\n",buflen);
    //                     memcpy(tx, udp_header,64);
    //                 }
    //                 // print_buf(tx,64);
    //                 memset(tx+64,0x04,buflen-64);
    //                 // print_buf(tx,buflen);

    //                 // send_packet(sockfd1, tx, buflen, sockaddrll1);
    //                 rx_to_tx(buflen);
    //                 // recev(buflen);
    //                 break;
    //             case 'q':
    //                 printf("exit\n");
    //                 return 0;
    //             default:
    //                 printf("please input 'c' to continue or q(exit)\n");
    //                 break;
    //         }
    //     }
    // }



    return 0;
}

void recev()
{
    memset(rx,0x00,1600);
    memcpy(rx,rtx,buflen);

    printf("\neno1 receved a packet\n");
    print_buf(rx, sizeof(rx));
}
void rx_to_tx()
{
    memset(rtx,0x00,1600);
    memcpy(rtx,tx,buflen);
    printf("\neno2 receved a packet\n");

    memcpy(rtx, MAC_ADDR_P, 6);
    print_buf(rtx, buflen);


}
