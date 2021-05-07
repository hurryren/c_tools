#include <linux/if_packet.h>
#include <stdint.h>


int open_raw_socket(const char *device, struct sockaddr_ll *sockaddrll);
int get_next_packet(int sockfd, uint8_t *buf, uint8_t *desmac, uint8_t *srcmac);
int send_packet(int sockfd, uint8_t *packet, int size, struct sockaddr_ll sockaddrll);


