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
    struct sockaddr_un ip;
};

struct cli_item{
    char want[50];
    char have[50];
};

int main(){
    char buf[256];
    struct sockaddr_un ser, cli;
    int sd, nsd, len, clen;
    struct cli_item *cli_item;
    struct item items[20];
    int items_num = 0;

    if((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        exit(1);
    }

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

    printf("Waiting...\n");
    if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
        perror("accept");
        exit(1);
    }

    if(recv(nsd, cli_item, sizeof(cli_item), 0) == -1){
        perror("recv");
        exit(1);
    }

    printf("Received want: %s\n", cli_item->want);
    printf("Received have: %s\n", cli_item->have);
    close(nsd);
    close(sd);
}