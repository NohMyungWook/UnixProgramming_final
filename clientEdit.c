#include <sys/socket.h>
#include <stdio.h>

#include "clientValues.h"

//식별 번호와 함께 서버에게 구조체를 전송하는 함수 
int sendStruct(int clientSD, struct AddItem newItem, int no){ 
    struct SendPacket newPacket; //패킷을 보낼 구조체 선언
    newPacket.no = no; //식별 번호 저장
    newPacket.nowItem = newItem; //보낼 구조체 저장
    
    //식별 번호가 포함한 구조체를 서버에게 패킷으로 보내기
    if (send(clientSD, (struct SendPacket*) &newPacket, sizeof(newPacket), 0) == -1) {
        perror("send"); //실패 시 오류 메시지 출력
        return 0; //0를 리턴
    }
	return 1; //성공 시 1를 리턴
}

//교환 물건을 등록하고 서버에게 보내는 함수
int createNew(int sd){
    struct AddItem newItem; //교환 물건 정보를 저장할 구조체 선언
    
    //원하는 물건을 입력 받기
    printf("원하는 물건을 입력하세요 : ");
    scanf("%s", newItem.want);

    //가지고 있는 물건을 입력 받기
    printf("가지고 있는 물건을 입력하세요 : ");
    scanf("%s", newItem.have);

    sendStruct(sd, newItem, 2); //식별 번호 2번(교환 정보 추가)과 함께 교환 등록 정보를 서버에게 전송

    printf("\n등록되었습니다. \n"); //안내 메시지 출력
    return 1; //1를 리턴
}
