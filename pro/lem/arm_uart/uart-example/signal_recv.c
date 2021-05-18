/* File Discription: Chanel signal receiver program
 * Functions: Receive signal message from uart, send it to RES-queue.
 * Author: Xiaoyan Ma
 * File Version: 
 *   1.00, 20210208. 
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <mqueue.h>

#include "util_tools.h"
#include "log.h"

#define MQ_NAME_REQ "/ExtraWriteBus"
#define MQ_NAME_RES "/InbandCommnadBus"
#define QUEUE_PERMISSIONS	0660

#define MAX_MESSAGES_COUNT	50
#define MAX_MSG_DATALEN		4096

#define TTY_DEVICENAME	"/dev/ttyS1"

extern int UART_Open(char* cDevicesName);
extern int UART_Recv(int fd, unsigned char *data, int datalen);

static int _RecvUnpackMessage(int fd, unsigned char *data, int *datalen)
{
    int rv = 0;
    unsigned char tmpbuf[2];
    int followedlen;
 
	// 2-byte, 0x7F7F
	for (;;)
	{
		rv = UART_Recv(fd, &tmpbuf[0], 1);
		if (rv < 0)
		{
			Log_Error("Read 1 from uart failed, rv = %d.", rv);
			return -1;
		}

		if (tmpbuf[0] != 0x7F)
			continue;

		rv = UART_Recv(fd, &tmpbuf[1], 1);
		if (rv < 0)
		{
			Log_Error("Read 2 from uart failed, rv = %d.", rv);
			return -1;
		}

		if (tmpbuf[1] != 0x7F)
			continue;
		else
			break;
	}

    // 2-byte, Followed Data Length
    rv = UART_Recv(fd, tmpbuf, 2);
	if (rv < 0)
	{
		Log_Error("Read 3 from uart failed, rv = %d.", rv);
		return -1;
	}

    followedlen = hex2short(tmpbuf);
 	if (*datalen < followedlen)
	{
		Log_Error("Parameter datalen[%d] is not enough, %d bytes expected.", *datalen, followedlen);
		return -3;
	}
 
    // followedlen-byte, Followed Data
    rv = UART_Recv(fd, data, followedlen);
	if (rv < 0)
	{
		Log_Error("Read 4 from uart failed, rv = %d.", rv);
		return -1;
	}

 	*datalen = followedlen;
    return 0;
}

int main()
{
	int rv;
	unsigned char msgbuf[MAX_MSG_DATALEN];
	int msglen;
	int serial_fd = 0;

	struct mq_attr	attr;
	mqd_t mq_res;

    struct timespec ts;
    ts.tv_sec = 3;
    ts.tv_nsec = 0;

	// Open MQID_NAME_REQ => qid_res
    attr.mq_maxmsg = MAX_MESSAGES_COUNT;
    attr.mq_msgsize = MAX_MSG_DATALEN;

    mq_res = mq_open(MQ_NAME_RES, O_RDWR);
    if (mq_res == -1)
    {
	    while ((mq_res = mq_open(MQ_NAME_RES, O_RDWR | O_CREAT | O_EXCL, QUEUE_PERMISSIONS, &attr)) == -1) 
	    {
	        printf("mq_open %s failed, errno=%d.\n", MQ_NAME_RES, errno);
	        sleep(1);
	    }
	}
	printf("mq_res = %d.\n", mq_res);

	// Open Serial TTY Device
	serial_fd = UART_Open(TTY_DEVICENAME);
	if (serial_fd < 0)
	{
		Log_Error("Init serial[%s] failed. Exit", TTY_DEVICENAME);
		return -2;
	}

	for (;;)
	{	
		// Get Message(chanel signals) from uart
		msglen = sizeof(msgbuf);
		rv = _RecvUnpackMessage(serial_fd, msgbuf, &msglen);
		if (rv)
		{
			Log_Error("Uart receive message failed. rv = %d.", rv);
			continue;
		}

	    rv = mq_timedsend(mq_res, msgbuf, msglen, 1, &ts);
		if (rv)
		{
			Log_Error("mq_timedsend failed, rv = %d. Continue", rv);
		}
	}

	return 0;
}


