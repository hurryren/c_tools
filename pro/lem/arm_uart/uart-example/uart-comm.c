/* File Discription: UART commnunication functions
 * Author: Xiaoyan Ma
 * File Version: 
 *   1.00, 20210208. 
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <fcntl.h> //�ļ����ƶ���
#include <termios.h>//�ն˿��ƶ���
#include <errno.h>

#include "log.h"
#include "util_tools.h"
#include "ipc.h"

#define BAUDRATE        B115200
#define MAX_TRY_TIMES	100	
#define FALSE  -1
#define TRUE   0

/**************************************************************
/* �򿪴��ڲ���ʼ������                            
/* cDevicesName:�豸����
**************************************************************/
int UART_Open(char* cDevicesName)
{
	int serial_fd;

    serial_fd = open(cDevicesName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd < 0) 
    {
       Log_Error("Open device[%s] failed, rv = %d.", cDevicesName, serial_fd);
       return -1;
    }
 
    //������Ҫ���ýṹ��termios <termios.h>
    struct termios options;
 
    /**1. tcgetattr�������ڻ�ȡ���ն���صĲ�����
    *����fdΪ�ն˵��ļ������������صĽ��������termios�ṹ����
    */
    tcgetattr(serial_fd, &options);
    /**2. �޸�����õĲ���*/
    options.c_cflag |= (CLOCAL | CREAD);//���ÿ���ģʽ״̬���������ӣ�����ʹ��
    options.c_cflag &= ~CSIZE;//�ַ����ȣ���������λ֮ǰһ��Ҫ�������λ
    options.c_cflag &= ~CRTSCTS;//��Ӳ������
    options.c_cflag |= CS8;//8λ���ݳ���
    options.c_cflag &= ~CSTOPB;//1λֹͣλ
    options.c_iflag |= IGNPAR;//����ż����λ
    options.c_iflag &= ~ICRNL;
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_oflag = 0; //���ģʽ
    options.c_lflag = 0; //�������ն�ģʽ
    cfsetospeed(&options, B115200);//���ò�����
 
    /**3. ���������ԣ�TCSANOW�����иı�������Ч*/
    tcflush(serial_fd, TCIFLUSH);//������ݿ��Խ��գ�������
    tcsetattr(serial_fd, TCSANOW, &options);
 
    return serial_fd;
}
 
/**************************************************************
/* ���ڷ�������                            
/* fd:����������
/* data:����������
/* datalen:���ݳ���
**************************************************************/
int UART_Send(int fd, unsigned char *data, int datalen)
{
    int len = 0, remainlen = datalen;
    unsigned char *p = data;
    int tryTimes = 0;

    for (tryTimes=0; tryTimes<MAX_TRY_TIMES; tryTimes++)
    {
        len = write(fd, p, remainlen);//ʵ��д��ĳ���
        if (len<0)
        {
            Log_Error("Write to uart failed, rv = %d.", len);
            return -1;
        }
        p += len;
        remainlen -= len;

        if (remainlen == 0)     //�������
            return 0;
    }

//    tcflush(fd, TCOFLUSH);//TCOFLUSHˢ��д������ݵ�������??
    return -1;
}

/**************************************************************
/* ���ڽ�������                            
/* Ҫ����������pc�˷���ascii�ļ�
/* fd:����������
/* data:�������ݴ洢��ַ
/* datalen:�������ݳ���
**************************************************************/
int UART_Recv(int fd, unsigned char *data, int datalen)
{
    int len=0, ret = 0, receivedlen;
    unsigned char *p = data;
    fd_set fs_read;
    struct timeval tv_timeout;
 
    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);
    tv_timeout.tv_sec  = (10*20/115200+2);
    tv_timeout.tv_usec = 0;
 
    //�������0��������������״̬�ı�ǰ�ѳ���timeoutʱ��,���󷵻�-1
    ret = select(fd+1, &fs_read, NULL, NULL, NULL);

    receivedlen = 0;
    while (receivedlen<datalen)
    { 
        if (FD_ISSET(fd, &fs_read)) 
        {
            len = read(fd, p, datalen-receivedlen);
           if (len >= 0) 
            {
                p += len;
                receivedlen += len;
            }
            else
            {
                if (errno == EAGAIN)
                {
                    select(fd+1, &fs_read, NULL, NULL, &tv_timeout);
                    continue;
                }
                else
                {
                    Log_Error("Receive return failed, errno = %d.\n", errno);
                    return -1;
                }
            }
        } 
    }

    return receivedlen;
}



