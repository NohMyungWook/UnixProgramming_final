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


int sendAddItemStruct(int clientSD, struct AddItem* newItem, int bufSize){
	if (send(clientSD, (struct AddItem*) newItem, bufSize, 0) == -1) {
        printf("send fail!");
        perror("send");
        return 0;
    }
    printf("send succsed!");
	return 1;

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

    strcpy(buf, "Hello, Market");
    close(sd);
}