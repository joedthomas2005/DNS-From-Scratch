#include<iphlpapi.h>
#include<stdlib.h>
#include<stdio.h>

typedef struct String{
    char* data;
    int length;
} String;

struct String getCurrentDNS(){
    FIXED_INFO* paramBuffer = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
    ULONG bufSize = sizeof(FIXED_INFO);
    GetNetworkParams(paramBuffer, &bufSize);
    String ipaddr = (String){ .data = paramBuffer->DnsServerList.IpAddress.String, .length = 16};
    return ipaddr;
};