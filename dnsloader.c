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
    cur->next->next = (Node*)NULL;
}

void printLinkedList(Node* head){
    Node* cur = head;
    while(cur){
        if(cur->next != NULL){
            printf("%c -> ", cur->character);
        }
        else{
            printf("%c", cur->character);
        }
        cur = cur->next;
    }
}

void freeLinkedList(Node* head){
    //WE CANNOT FREE HEAD NODES. HEAD NODES ARE ON STACK.
    Node* cur = head->next;
    printf("FREEING LINKED LIST: ");
    printLinkedList(head);
    printf("\n");
    while(cur){
        Node* prev = cur;
        cur = cur->next;
        free(prev);
    }
}
char* loadDNSFromFile(char* file){
    FILE* resolv = fopen(file, "r");
    char fileBuff[1024];
    fread(fileBuff, sizeof(char), sizeof(fileBuff), resolv);
    
    int i = 0;
    int numLines = 0;
    while(fileBuff[i]){
        i++;
        if(fileBuff[i] == '\n'){
            numLines++;
        }
    }
    const int length = i;

    if(fclose(resolv)){
        printf("FILE FAILED TO CLOSE");
        exit(EXIT_FAILURE); 
    };

    Node lines[numLines];
    lines[0] = (Node){.character = fileBuff[0], .initialised = 1, .next = (Node*)NULL};
    int line = 0;
    int character = 0;
    for(int i = 1; i < length; i++){
        char currentChar = fileBuff[i];
        if(currentChar == '\n'){
            line += 1;
        }
        else{
            if(lines[line].initialised == 1){
                addNode(&(lines[line]), fileBuff[i]);
            }
            else{
                lines[line] = (Node){.character = currentChar, .initialised = 1, .next = (Node*)NULL};
            }
        }
    }

    char* ipaddr = (char*)malloc(sizeof(char) * 16);
    int found = 0;
    printf("There are %i lines\n", numLines);
    for(int i = 0; i < numLines; i++){
        printf("On line: %i\n", i);
        if(lines[i].character != '#'){
            found = 1;
            Node* cur = &(lines[i]);
            while(cur->character != ' '){
                cur = cur->next;
            }
            cur = cur->next;
            int character = 0;
            while(cur){
                ipaddr[character] = cur->character;
                character++;
                cur = cur->next;
            }
            ipaddr[character + 1] = 0;
            freeLinkedList(&(lines[i]));
        }
        else{
            freeLinkedList(&(lines[i]));
        }
    }
    if(!found){
        ipaddr = "";
    }
    return ipaddr;
}

char* getCurrentDNS(){
    #ifdef onWindows
        FIXED_INFO* paramBuffer = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
        ULONG bufSize = sizeof(FIXED_INFO);
        GetNetworkParams(paramBuffer, &bufSize);        
        char* copy = (char*)malloc(sizeof(char) * 16);
        strcpy_s(copy, sizeof(char) * 16, paramBuffer->DnsServerList.IpAddress.String);
        free(paramBuffer);
        return copy;
    #else
        loadDNSFromFile("/etc/resolv.conf");
    #endif
};
