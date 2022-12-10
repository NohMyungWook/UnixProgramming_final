#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

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
    send(nsd, &items_num, sizeof(items_num), 0);
    for(int i=0; i<items_num; i++){
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
    strcpy(items[items_num].have, user_input->item.have);
    strcpy(items[items_num].want, user_input->item.want);
    return true;
}

void delete_item(struct item *items, int n, int items_num){
    for(int i=n; i<items_num; i++){
        memcpy(&items[i], &items[i+1], sizeof(struct item));
    }
    return;
}

int main(){
    struct sockaddr_un ser, cli;
    int sd, nsd, len, clen;
    struct item item;
    struct item items[50];
    struct user_input user_input;
    int items_num = 0;

    unlink(SOCKET_NAME);
    if((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }
    //test
    strcpy(items[items_num].want, "want1");
    strcpy(items[items_num].have, "have1");
    items_num++;

    strcpy(items[items_num].want, "want2");
    strcpy(items[items_num].have, "have2");
    items_num++;
    //end of test 

    memset((char*)&ser, 0, sizeof(struct sockaddr_un));
    ser.sun_family = AF_UNIX;
    strcpy(ser.sun_path, SOCKET_NAME);
    len = sizeof(ser.sun_family) + strlen(ser.sun_path);

    if(bind(sd, (struct sockaddr *)&ser, len)){
        perror("bind");
        exit(1);
    }
    if(listen(sd, 5) < 0){
    perror("listen");
    exit(1);
    }

    printf("Waiting for Client...\n");
    if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
        perror("accept");
        exit(1);
    }
    printf("Client connect!\n");
    send_item_list(nsd, items_num, items);
    while(1){
        if(recv(nsd, (struct user_input*)&user_input, sizeof(user_input), 0) == -1){
            perror("recv");
            exit(1);
        }
        switch(user_input.no){   
            case 1:
                for(int i=0; i<items_num; i++){
                    if((strcmp(items[i].want, user_input.item.want) + strcmp(items[i].have, user_input.item.have))==0){
                        printf("Delete List: index %d\n", i+1);
                        delete_item(items, i, items_num);
                        items_num --;
                    }
                }
                send_item_list(nsd, items_num, items);
                break;
            case 2:
                if(add_item(&user_input, items, items_num)){
                    items_num++;
                    printf("Add List: index %d\n", items_num);
                }
                send_item_list(nsd, items_num, items);
                break;
        }
    }
    close(nsd);
    close(sd);
}