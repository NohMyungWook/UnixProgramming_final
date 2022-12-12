#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define SOCKET_NAME "market"

struct item{
    char want[50];
    char have[50];
};

struct user_input{
    int no;
    struct item item;
};

void send_item_list(int nsd, int items_num, struct item *items){
    printf("Send list Start, List Size is %d\n", items_num);
    send(nsd, &items_num, sizeof(items_num), 0);
    for(int i=0; i<items_num; i++){
        printf("now Index is %d, now Have is %s\n", i, (items + i)->have);
        if(send(nsd, (struct item *)(items + i), sizeof(struct item), 0)== -1){
            perror("send");
            exit(1);
        }
    }
    return;
}

bool add_item(struct user_input *user_input, struct item *items, int items_num){
    if(items_num > 50){
        printf("Cannot add item\n");
        return false;
    }
    strcpy((items + items_num)->have, user_input->item.have);
    strcpy((items + items_num)->want, user_input->item.want);
    return true;
}

void delete_item(struct item *items, int n, int items_num){
    for(int i=n; i<items_num; i++){
        memcpy((items + i), (items + i+1), sizeof(struct item));
    }
    return;
}

int main(){

    int shmidList, shmidInt, i;
    struct item *shmaddrList;
    int* shmaddrInt;

    shmidList = shmget(IPC_PRIVATE, sizeof(struct item) * (50), IPC_CREAT|0644);
    if (shmidList == -1) {
        perror("shmget");
        exit(1);
    }

    shmidInt = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0644);
    if (shmidInt == -1) {
        perror("shmget");
        exit(1);
    }

    shmaddrList = (struct item*)shmat(shmidList, (struct item*)NULL, 0);
    shmaddrInt = (int * )shmat(shmidInt, (int *)NULL, 0);
    
    struct sockaddr_un ser, cli;
    int sd, len, clen;
    int nsd;
    struct item item;
    struct item* itemsListPointer = shmaddrList;

    int* items_num_Pointer = shmaddrInt;
    *items_num_Pointer = 0;

    struct user_input user_input;


    unlink(SOCKET_NAME);
    if((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
    //test
    strcpy((itemsListPointer + *items_num_Pointer)->want, "want1");
    strcpy((itemsListPointer + *items_num_Pointer)->have, "have1");
    *items_num_Pointer += 1;

    strcpy((itemsListPointer + *items_num_Pointer)->want, "want2");
    strcpy((itemsListPointer + *items_num_Pointer)->have, "have2");
    *items_num_Pointer += 1;
    //end of test 

    memset((char*)&ser, 0, sizeof(struct sockaddr_un));
    ser.sun_family = AF_UNIX;
    strcpy(ser.sun_path, SOCKET_NAME);
    len = sizeof(ser.sun_family) + strlen(ser.sun_path);

    if(bind(sd, (struct sockaddr *)&ser, len)){
        perror("bind");
        exit(1);
    }

    if(listen(sd, 10) < 0){
        perror("listen");
        exit(1);
    } 
    
    while(1){
        printf("Waiting for Client...\n");
        if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
            perror("accept");
            exit(1);
        }
        switch (fork()){
            case 0: // 자식 프로세스일 경우:

                shmaddrList = (struct item*)shmat(shmidList, (struct item*)NULL, 0);
                itemsListPointer = shmaddrList;
                shmaddrInt = (int * )shmat(shmidInt, (int *)NULL, 0);
                items_num_Pointer = shmaddrInt;

                printf("Client connect!\n");
                while(1){
                    if(recv(nsd, (struct user_input*)&user_input, sizeof(user_input), 0) == -1){
                        perror("recv");
                        exit(1);
                    }
                    else{
                        switch(user_input.no){   
                            case 0: 
                                send_item_list(nsd, (*items_num_Pointer), itemsListPointer);
                                break;
                            case 1:
                                for(int i=0; i<*items_num_Pointer; i++){
                                    if((strcmp((itemsListPointer + i)->want, user_input.item.want) + strcmp((itemsListPointer + i)->have, user_input.item.have))==0){
                                        printf("Delete List: index %d\n",i+1);
                                        delete_item(itemsListPointer, i, *items_num_Pointer);
                                        *items_num_Pointer -=1;
                                    }

                                }
                                break;
                            case 2:
                                if(add_item(&user_input, itemsListPointer, *items_num_Pointer)){
                                    *items_num_Pointer += 1;
                                    printf("Add List: index %d\n", *items_num_Pointer);
                                }
                                break;
                            case 3:
                                close(nsd);
                                exit(1);
                        }
                    }

                }
                
        }
        
    }
    close(nsd);
    close(sd);
}