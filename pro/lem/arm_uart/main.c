#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h> //文件控制定义
#include <termios.h>//终端控制定义
#include <errno.h>

#define BAUDRATE        B115200
#define FALSE  -1
#define TRUE   0

int serial_fd = 0;
char g_uartbuf[]="/dev/ttyS1";

int test_uart_port(char* cDeviceName);

int main(int argc, char **argv)
{
    int fd;
    int data_tx = 0x58FF;
    char *cDeviceName;
    int iUartNum = -1;

    srand(time(NULL));

    if (argc>1)
    	cDeviceName = argv[1];
    else
    	cDeviceName = g_uartbuf;

	test_uart_port(cDeviceName);
    return 0;
}

/**************************************************************
/* 打开串口并初始化设置
/* cDevicesName:设备名称
**************************************************************/
int init_serial(char* cDevicesName)
{
    serial_fd = open(cDevicesName, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd < 0) {
       perror("open");
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

    return 0;
}

/**************************************************************
/* 串口发送数据
/* fd:串口描述符
/* data:待发送数据
/* datalen:数据长度
**************************************************************/
int uart_send(int fd, unsigned char *data, int datalen)
{
    int len = 0;

    len = write(fd, data, datalen);//实际写入的长度
    if(len == datalen) {
        printf("uart send OK! len = %d.\n", len);
        return len;
    } else {
        tcflush(fd, TCOFLUSH);//TCOFLUSH刷新写入的数据但不传送
        return -1;
    }

    return 0;
}

/**************************************************************
/* 串口接收数据
/* 要求启动后，在pc端发送ascii文件
/* fd:串口描述符
/* data:接收数据存储地址
/* datalen:接收数据长度
**************************************************************/
int uart_recv(int fd, unsigned char *data, int datalen)
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
    ret = select(fd+1, &fs_read, NULL, NULL, &tv_timeout);

    receivedlen = 0;
    while (receivedlen<datalen)
    {
        if (FD_ISSET(fd, &fs_read)) {
            len = read(fd, p, datalen-receivedlen);
            if (len > 0) {
                p += len;
                receivedlen += len;
//                printf("len = %d. receivedlen = %d, datalen = %d.\n", len, receivedlen, datalen);
//                printbuf("try read", data, receivedlen);
            }
            else if (len == 0)
            {
                printf("Receive finished, receivedlen = %d.\n", receivedlen);
                return 0;
            }
            else
            {
                if (errno == EAGAIN)
                {
                    select(fd+1, &fs_read, NULL, NULL, &tv_timeout);
                    continue;
//                    return -2;
                }
                else
                {
                    printf("Receive return failed, errno = %d.\n", errno);
                    return -1;
                }
            }
        }
//            else {
//            return -1;
//        }
    }
    return 0;
}

void printbuf(char *title, unsigned char *buffer, int len)
{
    int i;

    printf("%s[%d]\n", title, len);
    for(i=0;i<len;i++)
    {
        printf("%02X ", buffer[i]);
        if (i%16==15)
            printf("\n");
    }
    printf("\n");
}

/**************************************************************
/* uartc测试函数
/* cDevicesName:设备名称
**************************************************************/
int test_uart_port(char* cDeviceName)
{
    int iRet = -1, i;
    unsigned char buf[1024];
    int buflen;
    unsigned char buf1[1024];
     int iTimeNum = 0;
    int iRet_Sen = -1;
    int iRet_Rec = -1;

    printf("UART Device Name : %s \n", cDeviceName);

    iRet = init_serial(cDeviceName);
    if (-1 == iRet)
    {
        printf("UART Test Result : FAIL \n");
        return -1;
    }

    memset(buf1, 0x0, sizeof(buf1));
    while (iTimeNum < 10)
    {
        // buflen = rand()%256;
        buflen =68; // header + 64 byte lsc
        for (i=0; i<buflen; i++)
            buf[i] = rand() & 0xFF;
        buf[0] = 0xf8; // header
        buf[1] = 0xf8;
        buf[2] = 0x00;
        buf[3] = 0x40;

        // buf[4] --- buf[15]   mac address
        // p1 和 p3过滤使用
        memset(buf+4,0x66,12);

        // buf[16] - buf[17]
        buf[16] = 0x08;
        buf[17] = 0x00;

        // ip header length
        buf[18] = 0x45;

        // tcp or udp flag
        buf[27] = 0x06; // 0x11 tcp

        // msg dest ip buf[34]---buf[37]
        buf[34] = 0x00;
        buf[35] = 0x00;
        buf[36] = 0x00;
        buf[37] = 0x00;

        // msg dest port buf[40]---buf[41]
        buf[40] = 0x00;
        buf[41] = 0x00;

        // lsc packet flag buf[54] -- buf[57]
        buf[54] = 0x00;
        buf[55] = 0x00;
        buf[56] = 0x00;
        buf[57] = 0x00;

        printbuf("will send", buf, buflen);

//            buf[i] = 'a'+i%26;//rand() & 0xFF;

        iRet_Sen = uart_send(serial_fd, buf, buflen);
//        printf("Try[%d], iRet_Sen = %d.\n", iTimeNum, iRet_Sen);

        iRet_Rec = uart_recv(serial_fd, buf1, buflen);
        if (iRet_Rec < 0)
        {
            printf("uart_recv failed, rv = %d.\n\n", iRet_Rec);
            close(serial_fd);
            return 1;
        }

        if (memcmp(buf,buf1,buflen))
        {
            printbuf("Received", buf1, buflen);
            printf("buf != buf1.\n\n");
            close(serial_fd);
            return -1;
        }

        printf("Loop-%d, UART Test Result : PASS. len = %d \n", iTimeNum, buflen);
//        printbuf("Received", buf1, iRet_Rec);

        iTimeNum++;
        sleep(0.2);
    }

    printf("UART Test Result : Finished \n");
    close(serial_fd);

    return 0;
}

