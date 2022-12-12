#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "clientValues.h"
#include "clientEdit.h"
#include "clientList.h"

#define SOCKET_NAME "market"

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
    
    //Send Client Pid
    struct AddItem sendPid;
    char clientPidChar[50];
    int clientPidInt = getpid();

    sprintf(clientPidChar, "%d", clientPidInt);
    strcpy(sendPid.have, clientPidChar);
    sendStruct(sd, sendPid, 9);

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
                printf("현재 물물교환이 가능한 물건이 없습니다\n");
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
            printf("잘못 입력했습니다. 정수 1, 2, 3중 하나를 입력하세요.\n ");
        }
        printf("\n");
    }

    struct AddItem empty;
    sendStruct(sd, empty, 3);
    close(sd);
}
