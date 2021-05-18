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
#include <fcntl.h> //文件控制定义
#include <termios.h>//终端控制定义
#include <errno.h>

#include "log.h"
#include "util_tools.h"
#include "ipc.h"

#define BAUDRATE        B115200
#define MAX_TRY_TIMES	100	
#define FALSE  -1
#define TRUE   0

/**************************************************************
/* 打开串口并初始化设置                            
/* cDevicesName:设备名称
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
 
    //串口主要设置结构体termios <termios.h>
    struct termios options;
 
    /**1. tcgetattr函数用于获取与终端相关的参数。
    *参数fd为终端的文件描述符，返回的结果保存在termios结构体中
    */
    tcgetattr(serial_fd, &options);
    /**2. 修改所获得的参数*/
    options.c_cflag |= (CLOCAL | CREAD);//设置控制模式状态，本地连接，接收使能
    options.c_cflag &= ~CSIZE;//字符长度，设置数据位之前一定要屏掉这个位
    options.c_cflag &= ~CRTSCTS;//无硬件流控
    options.c_cflag |= CS8;//8位数据长度
    options.c_cflag &= ~CSTOPB;//1位停止位
    options.c_iflag |= IGNPAR;//无奇偶检验位
    options.c_iflag &= ~ICRNL;
    options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    options.c_oflag = 0; //输出模式
    options.c_lflag = 0; //不激活终端模式
    cfsetospeed(&options, B115200);//设置波特率
 
    /**3. 设置新属性，TCSANOW：所有改变立即生效*/
    tcflush(serial_fd, TCIFLUSH);//溢出数据可以接收，但不读
    tcsetattr(serial_fd, TCSANOW, &options);
 
    return serial_fd;
}
 
/**************************************************************
/* 串口发送数据                            
/* fd:串口描述符
/* data:待发送数据
/* datalen:数据长度
**************************************************************/
int UART_Send(int fd, unsigned char *data, int datalen)
{
    int len = 0, remainlen = datalen;
    unsigned char *p = data;
    int tryTimes = 0;

    for (tryTimes=0; tryTimes<MAX_TRY_TIMES; tryTimes++)
    {
        len = write(fd, p, remainlen);//实际写入的长度
        if (len<0)
        {
            Log_Error("Write to uart failed, rv = %d.", len);
            return -1;
        }
        p += len;
        remainlen -= len;

        if (remainlen == 0)     //发送完成
            return 0;
    }

//    tcflush(fd, TCOFLUSH);//TCOFLUSH刷新写入的数据但不传送??
    return -1;
}

/**************************************************************
/* 串口接收数据                            
/* 要求启动后，在pc端发送ascii文件
/* fd:串口描述符
/* data:接收数据存储地址
/* datalen:接收数据长度
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
 
    //如果返回0，代表在描述符状态改变前已超过timeout时间,错误返回-1
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



