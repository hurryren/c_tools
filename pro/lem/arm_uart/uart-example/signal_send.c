/* File Discription: Chanel signal sender program
 * Functions: Receive signal message from REQ-queue, send the message to uart.
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
extern int UART_Send(int fd, unsigned char *data, int datalen);

static int _PackSendMessage(int fd, unsigned char *data, int datalen)
{
    unsigned char tmpbuf[2];
    int rv;

    // 2-byte, 0xF8F8
    tmpbuf[0] = 0x7F;		// only test, should be modified to 0xF8
    tmpbuf[1] = 0x7F;		// only test, should be modified to 0xF8
    rv = UART_Send(fd, tmpbuf, 2);
    if (rv)
    {
        Log_Error("Write 2-byte 0XF8 failed.");
        return rv;
    }

    // 2-byte, datalen
    tmpbuf[0] = datalen>>8 & 0xFF;
    tmpbuf[1] = datalen & 0xFF;
    rv = UART_Send(fd, tmpbuf, 2);
    if (rv)
    {
        Log_Error("Write 2-byte msglen[%d] failed.", datalen);
        return rv;
    }

    // n-byte, data
    rv = UART_Send(fd, data, datalen);
    if (rv)
    {
        Log_Error("Write %d-byte msg failed.", datalen);
        return rv;
    }

    return 0;
}

int main()
{
	int rv;
	unsigned char msgbuf[MAX_MSG_DATALEN];
	int msglen;
	int serial_fd = 0;

	struct mq_attr	attr;
	mqd_t mq_req, mq_res;

	// Open MQID_NAME_REQ => qid_req
    attr.mq_maxmsg = MAX_MESSAGES_COUNT;
    attr.mq_msgsize = MAX_MSG_DATALEN;

    mq_req = mq_open(MQ_NAME_REQ, O_RDWR);
    if (mq_req == -1)
    {
	    while ((mq_req = mq_open(MQ_NAME_REQ, O_RDWR | O_CREAT | O_EXCL, QUEUE_PERMISSIONS, &attr)) == -1) 
	    {
	        printf("mq_open %s failed, errno=%d.\n", MQ_NAME_REQ, errno);
	        sleep(1);
	    }
	}
	printf("mq_req = %d.\n", mq_req);

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
		// Get Message(chanel signals) from queue
        msglen = mq_receive(mq_req, (char *)msgbuf, MAX_MSG_DATALEN, NULL);
        if (-1 == msglen) 
        {
            Log_Error("mq_receive, errno: %d\n", errno);
            usleep(100000);
            continue;
        }

//		printbuf("Signal_send Buffer", msgbuf, msglen);
		// Send msg to FIFO through uart
		rv = _PackSendMessage(serial_fd, msgbuf, msglen);
		if (rv)
		{
			Log_Error("Uart send message[len=%d] failed. rv = %d.", msglen, rv);
		}
	}

	return 0;
}


