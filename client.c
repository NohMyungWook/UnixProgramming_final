#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SOCKET_NAME "market"

struct AddItem
{
	char want[50];
	char have[50];
};

int getCount(int sd){
    int nowCount;
    if(recv(sd, (int* )&nowCount, sizeof(nowCount), 0) == -1){
        perror("Count Recv");
        exit(1);
    }
    else{
        printf("받은 리스트 개수: %d\n", nowCount);
        return nowCount;
    }
    return -1;
}

int sendAddItemStruct(int clientSD, struct AddItem* newItem, int bufSize){
	if (send(clientSD, (struct AddItem*) newItem, bufSize, 0) == -1) {
        printf("send fail!\n");
        perror("send");
        return 0;
    }
    printf("send succsed!\n");
	return 1;
}

int get1Item(int sd, int count){
    
    for (int i = 0;i< count; i++){
        struct AddItem newItem;
        if(recv(sd, (struct AddItem * )&newItem, sizeof(newItem), 0) == -1){
        perror("recv");
        exit(1);
        }
        else{
            print("번호: %d: ", i+1);
            printf("가지고 있는 것: %s, ", newItem.have);
            printf("원하는 것: %s\n", newItem.want);
        }
    }
}


int main() {
    int sd, len;
    char buf[256];
    struct sockaddr_un ser;

    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&ser, '\0', sizeof(ser));
    ser.sun_family = AF_UNIX;
    strcpy(ser.sun_path, SOCKET_NAME);
    len = sizeof(ser.sun_family) + strlen(ser.sun_path);

    if (connect(sd, (struct sockaddr *)&ser, len) < 0) {
        perror("bind");
        exit(1);
    }

    int itemInt = getCount(sd);
    if (itemInt >= 0){
        get1Item(sd, itemInt);
    }else{
        printf("받은 리스트 개수가 0보다 작다.\n");
    }
	int testInt = 1;

	while(testInt == 1){
		struct AddItem newItem;
		
		printf("내가 가지고 있는거: ");
		scanf("%s", &newItem.have);
		
		printf("내가 원하는 거: ");
		scanf("%s", &newItem.want);

		sendAddItemStruct(sd, &newItem, sizeof(struct AddItem));
		printf("계속 보내고 싶어?(1/0): ");
		scanf("%d", &testInt);
	}

    close(sd);
}