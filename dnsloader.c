#if __has_include(<iphlpapi.h>)
#include<iphlpapi.h>
#define onWindows
#endif
#include<stdlib.h>
#include<stdio.h>

typedef struct Node {
    struct Node* next;
    unsigned char character;
    unsigned short int initialised;
} Node;

void addNode(Node* head, char value){
    Node* cur = head;
    while(cur->next){
        cur = cur->next;
    }
    cur->next = (Node*)malloc(sizeof(Node)); 
    cur->next->initialised = 1;
    cur->next->character = value;
}

void printLinkedList(Node* head){
    Node* cur = head;
    while(cur){
        if(cur->next){
            printf("%c -> ", cur->character);
        }
        else{
            printf("%c", cur->character);
        }
        cur = cur->next;
    }
}

char* getCurrentDNS(){
    #ifdef onWindows
        FIXED_INFO* paramBuffer = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
        ULONG bufSize = sizeof(FIXED_INFO);
        GetNetworkParams(paramBuffer, &bufSize);
        char* ipaddr = paramBuffer->DnsServerList.IpAddress.String;
        
        char* copy = (char*)malloc(sizeof(char) * 16);
        strcpy_s(copy, sizeof(char) * 16, paramBuffer->DnsServerList.IpAddress.String);
        free(paramBuffer);
        return copy;
    #else
        FILE* resolv = fopen("/etc/resolv.conf", "r");
        char fileBuff[1024];
        fread(fileBuff, sizeof(char), sizeof(fileBuff), resolv);
        int i = 0;
        while(fileBuff[i]){
            i++;
        }
        const int length = i;
        if(fclose(resolv)){
            printf("FILE FAILED TO CLOSE");
            exit(EXIT_FAILURE); 
        };

        int numLines = 0;
        for(int i =0; i < length;i++){
            if(fileBuff[i] == '\n'){
                numLines++;
            }
        }

        Node lines[numLines];
        lines[0] = (Node){.character = fileBuff[0], .initialised = 1};
        int line = 0;
        int character = 0;
        for(int i = 1; i < length; i++){
            char currentChar = fileBuff[i];
            if(currentChar == '\n'){
                line += 1;
            }
            else{
                if(lines[line].initialised == 1){
                    addNode(&lines[line], fileBuff[i]);
                    printLinkedList(&lines[line]);
                    printf("\n");
                }
                else{
                    lines[line] = (Node){.character = currentChar, .initialised = 1};
                }
            }
        }

        char* ipaddr = (char*)malloc(sizeof(char) * 16);
        for(int i = 0; i < numLines; i++){
            if(lines[i].character != "#"){
                Node* cur = &lines[i];
                while(cur->character != " ");
                    cur = cur->next;
                int character = 0;
                while(cur){
                    ipaddr[character] = cur->character;
                    character++;
                }
                ipaddr[character + 1] = 0;
            }
        }

        return ipaddr;

    #endif
};