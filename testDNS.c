#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
extern char* getCurrentDNS();
extern char* loadDNSFromFile(char* path);
int main(){
    printf("PLATFORM NATIVE OUTPUT\n");
    char* ipaddr = loadDNSFromFile("/etc/resolv.conf");
    puts(ipaddr);
    ipaddr = loadDNSFromFile("/etc/resolv.conf");
    puts(ipaddr);
    return 0;
}