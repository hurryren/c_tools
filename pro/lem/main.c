#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
// #include <netpacket/packet.h>





void  send_pkt(unsigned char *tx_data);
void  receive();
void print_buf(unsigned char *buf, int len);

char ifName[IFNAMSIZ];

unsigned char udp_msg[1024] = {
		//--------------组MAC--------14------
		0x74, 0x27, 0xea, 0xb5, 0xef, 0xd8, //dst_mac: 74-27-EA-B5-FF-D8
		0xc8, 0x9c, 0xdc, 0xb7, 0x0f, 0x19, //src_mac: c8:9c:dc:b7:0f:19
		0x08, 0x00,                         //类型：0x0800 IP协议
		//--------------组IP---------20------
		0x45, 0x00, 0x00, 0x00,             //版本号：4, 首部长度：20字节, TOS:0, --总长度--：
		0x00, 0x00, 0x00, 0x00,				//16位标识、3位标志、13位片偏移都设置0
		0x80, 17,   0x00, 0x00,				//TTL：128、协议：UDP（17）、16位首部校验和
		10,  221,   20,  11,				//src_ip: 10.221.20.11
		10,  221,   20,  10,				//dst_ip: 10.221.20.10
		//--------------组UDP------- 8-------
		0x1f, 0x90, 0x1f, 0x90,             //src_port:0x1f90(8080), dst_port:0x1f90(8080)
		0x00, 0x00, 0x00, 0x00,               //#--16位UDP长度--30个字节、#16位校验和
        //--------------payload------18--------
        0x01,0x02, 0x03, 0x04,             //版本号：4, 首部长度：20字节, TOS:0, --总长度--：
        0x05,0x06, 0x07, 0x08,
        0x00, 0x00, 0x00, 0x00, // lsc flag
        0x0d, 0x0e, 0x0f, 0x10,
        0x11,0x12
};

#define MY_DEST_MAC0	0x00
#define MY_DEST_MAC1	0x00
#define MY_DEST_MAC2	0x00
#define MY_DEST_MAC3	0x00
#define MY_DEST_MAC4	0x00
#define MY_DEST_MAC5	0x00


int main(int argc, char *argv[]){

	int sockfd;

    /* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName,"eno1");


    send_pkt(udp_msg);






}


void  send_pkt(unsigned char *tx_data){
    int sockfd;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	struct ether_header *eh = (struct ether_header *) tx_data;
	struct iphdr *iph = (struct iphdr *) (tx_data + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;


	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	// memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_P_IP);
	tx_len += sizeof(struct ether_header);


	/* Index of the network device */
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	/* Address length*/
	socket_address.sll_halen = ETH_ALEN;
	/* Destination MAC */
	socket_address.sll_addr[0] = MY_DEST_MAC0;
	socket_address.sll_addr[1] = MY_DEST_MAC1;
	socket_address.sll_addr[2] = MY_DEST_MAC2;
	socket_address.sll_addr[3] = MY_DEST_MAC3;
	socket_address.sll_addr[4] = MY_DEST_MAC4;
	socket_address.sll_addr[5] = MY_DEST_MAC5;

    print_buf(tx_data,60);

	/* Send packet */
	if (sendto(sockfd, tx_data, 60, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    printf("Send failed\n");


}


void  receive(){

}

void print_buf(unsigned char *buf, int len)
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