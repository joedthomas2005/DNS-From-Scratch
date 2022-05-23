#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
extern char* getCurrentDNS();
int main(){
    printf("LOADING DNS CONFIGURATION\n");
    char* ipaddr = getCurrentDNS();
    puts(ipaddr);
    return 0;
}