#if __has_include(<iphlpapi.h>)
#include<iphlpapi.h>
#define onWindows
#endif
#include<stdlib.h>
#include<stdio.h>

typedef struct String{
    char* data;
    unsigned int length;
} String;

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
        String ipaddr = (String){ .data = paramBuffer->DnsServerList.IpAddress.String, .length = 16};
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

        printf("CREATING LINKED LISTS\n");
        Node lines[numLines];
        lines[0] = (Node){.character = fileBuff[0], .initialised = 1};
        int line = 0;
        int character = 0;
        for(int i = 1; i < length; i++){
            char currentChar = fileBuff[i];
            if(currentChar == '\n'){
                printf("NEW LINE\n");
                line += 1;
            }
            else{
                if(lines[line].initialised == 1){
                    printf("ADDING CHARACTER %c\n", fileBuff[i]);
                    addNode(&lines[line], fileBuff[i]);
                    printLinkedList(&lines[line]);
                    printf("\n");
                }
                else{
                    printf("INITIALISING NEW LINE\n");
                    lines[line] = (Node){.character = currentChar, .initialised = 1};
                }
            }
        }

    #endif
    return paramBuffer->DnsServerList.IpAddress.String;
};
