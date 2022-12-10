#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_NAME "market"

struct item{
    char want[50];
    char have[50];
};

struct cli_item{
    char want[50];
    char have[50];
};


int main(){
    char buf[256] = "test";
    struct sockaddr_un ser, cli;
    int sd, nsd, len, clen;
    struct cli_item cli_item;
    struct item items[50];
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
    while(1){
        if(items_num > 50){
            printf("Cannot add item in list\n");
            items_num--;
        }
        else{
            if(listen(sd, 5) < 0){
            perror("listen");
            exit(1);
            }

            printf("Waiting...\n");
            if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
                perror("accept");
                exit(1);
            }
            else{
                send(nsd, &items_num, sizeof(items_num), 0);
                for(int i=0; i<items_num; i++){
                    printf("%d\n", i);
                    printf("%s, %s\n", items[i].have, items[i].want);
                    if(send(nsd, (struct item *)(items + i), sizeof(struct item), 0)== -1){
                        perror("send");
                        exit(1);
                    }
                }
            }

            if(recv(nsd, (struct cli_item*)&cli_item, sizeof(cli_item), 0) == -1){
                perror("recv");
                exit(1);
            }
            else{
                strcpy(items[items_num].have, cli_item.have);
                strcpy(items[items_num].want, cli_item.want);
                
            }

            printf("Received want: %s\n", items[items_num].want);
            printf("Received have: %s\n", items[items_num].have);
            items_num++;
        }
        
    }
    close(nsd);
    close(sd);
}