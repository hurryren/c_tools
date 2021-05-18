#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <mqueue.h>

#include "util_tools.h"

#define MQ_NAME_REQ "/ExtraWriteBus"
#define MQ_NAME_RES "/InbandCommnadBus"

#define MAX_MESSAGES_COUNT	100
#define MAX_MSG_DATALEN		4096

int AssembleSignalMsg(char *pcJson, unsigned char *pucSignalMsg/*out*/, int *piMsgLen)
{
	unsigned char *p = pucSignalMsg;

	// 4-byte, 0x22222222
	memset(p, 0x22, 4);
	p += 4;

	// 8-byte, reserved
	memset(p, 0x0, 8);
	p += 8;

	// 4-byte, followed data length
	uint2hex(22+4+strlen(pcJson), p );
	p += 4;

	// MAC header: 8-byte, 0x55555555 0x555555d5
	memset(p, 0x55, 7);
	p += 7;
	*p++ = 0xd5;

	// MAC header: 6-byte, source MAC address
	memset(p, 0x11, 6);
	p += 6;

	// MAC header: 6-byte, destinate MAC address
	memset(p, 0x22, 6);
	p += 6;

	// MAC header: 2-byte, IPV4 - 0x0800
	*p++ = 0x08;
	*p++ = 0x00;

	// 4-byte, "TASS"
	memcpy(p, "TASS", 4);
	p += 4;

	// n-byte, JSON string
	memcpy(p, pcJson, strlen(pcJson));
	p += strlen(pcJson);

	*piMsgLen = p - pucSignalMsg;

	return 0;
}

int main()
{
	int rv, i, j;

	int iAsciiLen, iMsgLen;
	unsigned char pucMsg[4096], *p;
	char pcJson[2048];

	mqd_t mq_res;

	// Open MQID_NAME_RES => qid_req
    while ((mq_res = mq_open(MQ_NAME_RES, O_RDONLY)) == -1) 
    {
        printf("mq_open %s failed, errno=%d.\n", MQ_NAME_RES, errno);
        sleep(1);
    }
    printf("mq_res = %d.\n", mq_res);

	for (i=0; ;i++)
	{
		memset(pucMsg, 0x0, sizeof(pucMsg));
        iMsgLen = mq_receive(mq_res, (char *)pucMsg, MAX_MSG_DATALEN, NULL);
        if (-1 == iMsgLen) 
		{
			printf("[loop-%d]Get message from mq_res failed. rv = %d.\n", i, iMsgLen);
			return -1;
		}

		printf("[loop-%d]Get message from mq_res OKOK.\n", i);

		printbuf("Receive message", pucMsg, 42);
		printf("Json string: %s.\n\n", &pucMsg[42]);
	}

	return 0;
}






