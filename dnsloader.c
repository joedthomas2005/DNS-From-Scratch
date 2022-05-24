#if __has_include(<Windows.h>)
#include<Windows.h>
#include<iphlpapi.h>
#define onWindows
#endif
#include<stdlib.h>
#include<stdio.h>

typedef struct Node {
    /**
     * Represents a node in a linked list.
     */
    struct Node* next;
    unsigned char character;
    unsigned short int initialised;
} Node;

void addNode(Node* head, char value){
    /**
     * Add a new node to a linked list given the head node.
     * Goes through all nodes until the last node is found and then a new 
     * node is created on heap for that one to point to.
     */
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
    /**
     * Output a linked list in standard
     * A -> B -> C -> D form given the head node
     */
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
    /**
     * Goes through a linked list and frees all nodes past the head 
     * to avoid the large memory leaks when freeing only the head nodes.
     * As links are pointers to nodes created on heap, 
     * when the head node is freed all the others become inaccessible.
     * Make sure to call this *before* freeing the head of a linked list.
     */
    Node* cur = head->next;
    while(cur){
        Node* prev = cur;
        cur = cur->next;
        free(prev);
    }
}
char* loadDNSFromFile(char* file){
    /**
     * Load and parse a file with resolv.conf syntax. 
     * This means # comments and the DNS server in the form 
     * "nameserver x.x.x.x". 
     */

    FILE* resolv;
    fopen_s(&resolv, file, "r");
    fseek(resolv, 0, SEEK_END);
    long fsize = ftell(resolv);
    fseek(resolv, 0, SEEK_SET);

    char* fileBuff = (char*)malloc(fsize + 1);
    fread(fileBuff, fsize, 1, resolv);
    
    if(fclose(resolv)){
        printf("FILE IO ERROR\n");
        exit(EXIT_FAILURE); 
    };
    
    fileBuff[fsize] = 0;

    int i = 0;
    int numLines = 1; 
    while(fileBuff[i]){
        i++;
        if(fileBuff[i] == '\n'){
            numLines++;
        }
    }
    const int length = i;
    Node* lines = (Node*)malloc(sizeof(Node) * numLines);

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
    free(fileBuff);

    char* ipaddr = (char*)malloc(sizeof(char) * 16);
    int found = 0;
    for(int i = 0; i < numLines; i++){
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
            ipaddr[character] = 0;
            freeLinkedList(&(lines[i]));
        }
        else{
            freeLinkedList(&(lines[i]));
        }
    }
    free(lines);
    if(!found){
        ipaddr = "";
    }
    return ipaddr;
}

char* getCurrentDNS(){
    /**
     * Get the current DNS server from the OS network config.
     */
    #ifdef onWindows
        FIXED_INFO* paramBuffer = (FIXED_INFO*)malloc(sizeof(FIXED_INFO));
        ULONG bufSize = sizeof(FIXED_INFO);
        GetNetworkParams(paramBuffer, &bufSize);        
        char* copy = (char*)malloc(sizeof(char) * 16);
        strcpy_s(copy, sizeof(char) * 16, paramBuffer->DnsServerList.IpAddress.String);
        free(paramBuffer);
        return copy;
    #else
        return loadDNSFromFile("/etc/resolv.conf");
    #endif
};
