#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/ioctl.h>


#include<unistd.h>
#include<pthread.h>
#include<sys/time.h>
#include<time.h>
#include<sys/mman.h>
#include<errno.h>

#define WRITE_CH0   0xc001
#define CTRL_USER   0xc020

#define FATAL do {fprintf(stderr, "error at line %d, file %s (%d) [%s]\n", __LINE__, __FILE__, errno, strerror(errno)); exit(1);}while(0)

unsigned int g_seconds = 0;
unsigned int g_write_total = 0;
unsigned long long g_write_len = 0;

unsigned char g_scrambler[10240];

void printbuf(unsigned char *buf, int len){
    int i;
    for (i=0;i<len;i++){
        printf("%02X",buf[i]);
    if((i+1) % 20 == 0)
        printf("\n");
    } 
    printf("\n");

}

void *twatch(void *param){
    float speed;
    unsigned int old_write_total = 0;
    unsigned long long old_write_len = 0;
    while(1){
        speed = (float)(g_write_len - old_write_len) * 8 / 1000/ 1000;
        printf("[sec:%d] wcnt: %d, wlen: %lld, speed:%0.2f\n",g_seconds,g_write_total-old_write_total, g_write_len,speed);
        old_write_total = g_write_total;
        old_write_len = g_write_len;
        sleep(1);
        g_seconds++;
    
    }
}

// 读 status
u_int16_t g_status;
void *watch_status(void *map_base){
    u_int16_t old_status = 0x0000;
    void *virt_addr;
    virt_addr = map_base + 0x4000;
    while(1){
        g_status = *((u_int16_t *) virt_addr);
        if (g_status != old_status){
            printf("[sec:%d] status is : %04x\n",g_seconds,g_status);
            old_status = g_status;
        }
        sleep(1);
        g_seconds++;
    }
    
}


int main(int argc, char *argv[]){
  // 映射 bar0 地址
    int rv_reg;
    int fd_reg;
    void *eth_map_base, *virt_addr;


    if((fd_reg = open("/dev/tass_sdh",O_RDWR|O_SYNC)) == -1)
            FATAL;
    printf("bar0 character device opened\n");
    fflush(stdout);

    if(rv_reg = ioctl(fd_reg,CTRL_USER, NULL))
        FATAL;

    eth_map_base = mmap(0,32768, PROT_READ | PROT_WRITE, MAP_SHARED, fd_reg, 0);
    if(eth_map_base == (void *)-1)
            FATAL;
    printf("bar0 memeory maped at address %p.\n", eth_map_base);
    fflush(stdout);

    // 检查 status
    pthread_t tid;
    pthread_create(&tid,0,(void *)twatch,(void *)(eth_map_base));

    // 发送通道选择信号和 remote update 启动信号
    u_int8_t write_val = 0xff;
    virt_addr = eth_map_base + 0x4000;
    *((u_int8_t *) virt_addr) = write_val;
    printf("write 8-bits value 0x%02x to 0x%08x (0x%p)\n",(unsigned int) write_val,0x4000,virt_addr);

    // 等待 status 显示 erase update area finish
    while(1){
        if(g_status == 0x000f){
            break;
        }else{
            sleep(1);
        }
    }

    // 发送 8MB 更新 bin 文件
    unsigned char msg[4096];
    int update_file_fd = open("./update.bin",O_RDONLY);
    if(update_file_fd < 0){
        FATAL;
    }else{
        printf("open update file success!");
    }

    int fd_w,rv;
    unsigned int txrole = WRITE_CH0;

    if((fd_w = open("/dev/tass_sdh",O_RDWR)) < 0){
        FATAL;
    }
    printf("open tass_sdh success\n");

    rv = ioctl(fd_w, txrole, NULL);
    if(rv){
        FATAL;
    }
    printf("send update file rv sucess\n");

    int count=0;
    while(count < 8096){
        rv = read(update_file_fd,msg,1024);
        if(rv < 0)
            FATAL;
        rv = write(fd_w,msg,1024);
        if(rv != 1024)
            FATAL;
        
        count++;
        sleep(0.1);
    }

    // 等待完成
    while(1){
        if(g_status == 0x003f){
            printf("update finish!!!");
            break;
        }else{
            sleep(1);
        }
    }

}


