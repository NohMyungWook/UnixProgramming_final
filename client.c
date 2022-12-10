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

struct SendPacket
{
    int no;
    struct AddItem nowItem;
};

int sendStruct(int clientSD, struct AddItem newItem, int no){
	//보낼 패킷을 만들기
    struct SendPacket newPacket;
    newPacket.no = no;
    newPacket.nowItem = newItem;
    
    if (send(clientSD, (struct SendPacket*) &newPacket, sizeof(newPacket), 0) == -1) {
        perror("send");
        return 0;
    }
	return 1;
}

int getCount(int sd){
    int nowCount;
    if(recv(sd, (int* )&nowCount, sizeof(nowCount), 0) == -1){
        perror("Count Recv");
        exit(1);
    }
    else{
        return nowCount;
    }
    return -1;
}

int get1Item(int sd, int count, struct AddItem* allItemList){
    
    for (int i = 0;i< count; i++){
        struct AddItem newItem;
        memset((struct AddItem *)&newItem, '\0', sizeof(struct AddItem));

        if(recv(sd, (struct AddItem * )&newItem, sizeof(struct AddItem), 0) == -1){
        perror("recv");
        exit(1);
        }
        else{
            *(allItemList + i) = newItem;
            printf("[%d]\n", i+1);
            printf("원하는 물건: %s\n", newItem.want);
            printf("갖고 싶은 물건: %s\n", newItem.have);
        }
    }
}

int printNowList(int sd, struct AddItem* allItemList){
    printf("-----현재 물물교환 가능 리스트-----\n");

    int itemInt = getCount(sd);

    if (itemInt > 0){
        get1Item(sd, itemInt, allItemList);
    }else{
        printf("현재 물물교환이 가능한 물건이 없습니다..\n");
    }

    printf("---------------------------------------\n");
    return itemInt;
}

int createNew(int sd){
    struct AddItem newItem;
    
    printf("원하는 물건을 입력하세요 : ");
    scanf("%s", newItem.want);

    printf("가지고 있는 물건을 입력하세요 : ");
    scanf("%s", newItem.have);

    sendStruct(sd, newItem, 2);

    printf("\n등록되었습니다. \n");
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

    //Connect End, Program Start
    int isRun = 1;
    while(isRun){

        struct AddItem allItemList[50];

        //리스트 출력 구간
        int nowListSize = printNowList(sd, allItemList);

        printf("(1)교환 (2)등록 (3)나가기\n");
        printf("원하시는 작업을 선택하세요 : ");
        int taskNum = 0;
        scanf("%d", &taskNum);

        if(taskNum == 1){
            if(nowListSize <=0){
                printf("현재 물물교환이 가능한 물건이 없습니다");
                continue;
            }
            printf("교환을 원하시는 번호를 입력해주세요 : ");
            int changeNum = 0;
            scanf("%d", &changeNum);
            if(changeNum > 0 && changeNum <= nowListSize){
                //보낼 item의 구조체를 만들기
                int changeIndex = changeNum - 1;
                struct AddItem sendItem;
                strcpy(sendItem.have, allItemList[changeIndex].have);
                strcpy(sendItem.want, allItemList[changeIndex].want);

                sendStruct(sd, sendItem, 1);

                printf("\n교환이 완료되었습니다.\n");

            }else{
                printf("번호가 범위 안에 없습니다.\n");
            }
        }else if (taskNum == 2){
            createNew(sd);
        }else if(taskNum == 3){
            printf("이용해주셔서 감사합니다. \n");
            isRun = 0;
        }else{
            printf("잘못 입력했습니다. 정수 1, 2, 3중 하나를 입력하세요. ");
        }
        printf("\n");
    }
    close(sd);
}


// int sendAddItemStruct(int clientSD, struct AddItem* newItem, int bufSize){
// 	if (send(clientSD, (struct AddItem*) newItem, bufSize, 0) == -1) {
//         perror("send");
//         return 0;
//     }
// 	return 1;
// }