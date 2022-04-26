#include<stdio.h>
#include<Windows.h>

int main(){
    int cnt = 5;

    while(cnt >= 0){
        Sleep(1000);
        printf("%d\r",cnt);
        cnt--;
    }
    return 0;

}