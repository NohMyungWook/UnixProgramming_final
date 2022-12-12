// #include <sys/socket.h>
// #include <sys/un.h>
// #include <stdlib.h>
// #include <stdio.h>

//교환 정보 리스트 크기를 패킷으로 받고 반환하는 함수
int getCount(int sd){
    int nowCount; //리스트 크기를 저장하는 변수 선언
    //리스트 크기 정보가 있는 패킷을 받기
    if(recv(sd, (int* )&nowCount, sizeof(nowCount), 0) == -1){
        perror("Count Recv"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }
    else{//성공 시
        return nowCount; // 리스트 크기를 리턴
    }
    return -1; //실패 시 -1를 리턴
}

//교환 정보 리스트를 받고 해당 정보를 저장 및 출력
int get1Item(int sd, int count, struct AddItem* allItemList){
    //교환 정보 리스트 크기 만큼 반복
    for (int i = 0;i< count; i++){
        struct AddItem newItem; //교환 정보를 저장할 구조체 선언
        memset((struct AddItem *)&newItem, '\0', sizeof(struct AddItem)); //해당 구조체를 초기화
        //교환 정보가 담긴 구조체를 패킷을 받기
        if(recv(sd, (struct AddItem * )&newItem, sizeof(struct AddItem), 0) == -1){
            perror("recv"); //실패 시 오류 메시지 출력
            exit(1); //프로세스 종료
        }
        else{ //성공 시
            *(allItemList + i) = newItem; //교환 정보 리스트에 받은 교환 정보를 저장
            //해당 교환 정보를 출력
            printf("[%d]\n", i+1);
            printf("원하는 물건: %s\n", newItem.want);
            printf("갖고 싶은 물건: %s\n", newItem.have);
        }
    }
}

//교환 정보 리스트를 출력 및 저장를 관리하는 함수
int printNowList(int sd, struct AddItem* allItemList){
    struct AddItem empty; //빈 구조체를 선언하고
    sendStruct(sd, empty, 0); ///식별 번호 0번(교환 정보 리스트 크기 요청)과 함께 PID정보를 서버에게 전송

    printf("-----현재 물물교환 가능 리스트-----\n");

    int itemInt = getCount(sd); //교환 정보 리스트 크기 반환 받기

    if (itemInt > 0){ //교환 정보 리스트 크기가 0보다 크면
        get1Item(sd, itemInt, allItemList); //전체 교환 정보 리스트 저장 및 출력
    }else{ //교환 정보 리스트 크기가 0이하 이면
        printf("현재 물물교환이 가능한 물건이 없습니다..\n\n"); //안내 메시지 출력
    }

    printf("---------------------------------------\n");
    return itemInt; //교환 정보 리스트 크기를 반환
}
