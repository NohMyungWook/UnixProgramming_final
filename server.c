#include <sys/shm.h>
#include <stdbool.h>

#include "serverValues.h"
#include "serverList.h"
#include "serverSemaphore.h"

#define SOCKET_NAME "market"


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
    
    printf("Waiting for Client...\n");
    while(1){
        clen = sizeof(cli);
        if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
            perror("accept");
            exit(1);
        }
        switch (fork()){
            case -1:
                perror("fork");
                exit(1);
            case 0: // 자식 프로세스일 경우:
                shmaddrList = (struct item*)shmat(shmidList, (struct item*)NULL, 0);
                itemsListPointer = shmaddrList;
                shmaddrInt = (int * )shmat(shmidInt, (int *)NULL, 0);
                items_num_Pointer = shmaddrInt;
                int semid = initsem(1);
                char client_pid[50];
                if(semid < 0)
                    exit(1);
                printf("New Client connect!\n");
                while(1){
                    if(recv(nsd, (struct user_input*)&user_input, sizeof(user_input), 0) == -1){
                        perror("recv");
                        exit(1);
                    }
                    else{
                        switch(user_input.no){   
                            case 0: 
                                send_item_list(nsd, (*items_num_Pointer), itemsListPointer, (int)getpid());
                                break;
                            case 1:
                                semlock(semid);
                                for(int i=0; i<*items_num_Pointer; i++){
                                    if((strcmp((itemsListPointer + i)->want, user_input.item.want) + strcmp((itemsListPointer + i)->have, user_input.item.have))==0){
                                        printf("Delete List: index %d\n",i+1);
                                        delete_item(itemsListPointer, i, *items_num_Pointer);
                                        *items_num_Pointer -=1;
                                    }

                                }
                                semunlock(semid);
                                break;
                            case 2:
                                semlock(semid);
                                if(add_item(&user_input, itemsListPointer, *items_num_Pointer)){
                                    *items_num_Pointer += 1;
                                    printf("Add List: index %d\n", *items_num_Pointer);
                                }
                                semunlock(semid);
                                break;
                            case 3:
                                printf("Client disconnect! # of Proc: %d\n\n", (int)getpid());
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