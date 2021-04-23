#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#define cpu_to_be32(v) (((v)>>24) | (((v)>>8)&0xff00) | (((v)<<8)&0xff0000)|((v)<<24))


static void int2char(const unsigned int src, unsigned char *dst){
    unsigned  int temp = src;
    if(NULL == dst)
        return;
    for(int i=0;i<4;i++){
        dst[i] = temp&0xff;
        temp = temp>>8;
    }
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

int Hex2Bin(const char *src, unsigned char *dst)
{
    int i = 0, j, len;
    unsigned int temp;
    if (NULL == src)
        return -1;

    int srclen = strlen(src);
    if (srclen == 0)
        return -1;
    for (i = 0; i < srclen; i += 2)
    {
        for (j = 0; j < 2; j++)
        {
            if (src[i + j] >= '0' && src[i + j] <= '9')
                continue;
            if (src[i + j] >= 'a' && src[i + j] <= 'f')
                continue;
            if (src[i + j] >= 'A' && src[i + j] <= 'F')
                continue;
            else
                return -1;
        }
        if(sscanf(&(src[i]), "%02x",&temp) >0){
            dst[i/2] = temp;
        }else
            break;

            len = i / 2;

            return len;

    }
}
