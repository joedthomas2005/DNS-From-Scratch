#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
extern char* getCurrentDNS();
extern char* loadDNSFromFile(char* path);
int main(){
    printf("LOADING DNS CONFIGURATION\n");
    char* ipaddr = loadDNSFromFile("sample.resolv.conf");
    puts(ipaddr);
    return 0;
}