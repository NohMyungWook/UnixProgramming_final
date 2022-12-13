#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "clientValues.h"
#include "clientEdit.h"
#include "clientList.h"

int main() {
    //소켓 프로그래밍을 위함 변수 선언
    int sd, len; //소켓 기술자, 소켓 주소 구조체 크기
    struct sockaddr_un ser; //소켓 주소 구조체

    //소켓 파일 기술자 생성
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }

    //소켓 주소 구조체 초기화
    memset((char *)&ser, '\0', sizeof(ser));
    ser.sun_family = AF_UNIX; //유닉스 도메인 소켓 지정
    strcpy(ser.sun_path, SOCKET_NAME); //경로 지정
    len = sizeof(ser.sun_family) + strlen(ser.sun_path); //구조체 크기 계산

    //클라이언트가 서버에 접속 요청
    if (connect(sd, (struct sockaddr *)&ser, len) < 0) {
        perror("bind"); //실패 시 오류 출력
        exit(1); //종료
    }
    
    //서버와 연결 성공
    //먼저 서버에게 클라이언트의 PID를 패킷으로 전송
    struct AddItem sendPid; //전송할 구조체 선언
    int clientPidInt = getpid(); //클라이언트의 PID 검색
    char clientPidChar[50]; //PID를 저장할 문자열 선언
    sprintf(clientPidChar, "%d", clientPidInt); //PID를 문자열으로 바꾸기
    strcpy(sendPid.have, clientPidChar); //구조체에 문자열인 PID를 저장
    sendStruct(sd, sendPid, 9); //식별 번호 9번(클라이언트 PID 전송)과 함께 PID정보를 서버에게 전송

    //클라이언트 초기 설정 완료//

    //교환 정보 출력 및 메뉴 실행
    int isRun = 1; //연속 실행 여부 정의(0: 실행 종료, 1: 다시 실행 )
    while(isRun){ 
        //연속 실행 구간
        struct AddItem allItemList[50]; //교환 정보 리스트 선언

        //교환 정보 리스트를 출력하고, 리스트의 총 개수를 반환
        int nowListSize = printNowList(sd, allItemList);

        //메뉴 출력 및 입력 받기
        printf("(1)교환 (2)등록 (3)나가기\n");
        printf("원하시는 작업을 선택하세요 : ");
        int taskNum = 0;
        scanf("%d", &taskNum);

        if(taskNum == 1){ //선택한 메뉴가 1번 "교환"일 경우
            //현재 교환 정보 리스트의 개수가 0이하이면
            if(nowListSize <=0){
                printf("현재 물물교환이 가능한 물건이 없습니다\n");
                continue; //이번 실행을 바로 종료하고 메뉴 재실행
            }

            printf("교환을 원하시는 번호를 입력해주세요 : "); 
            int changeNum = 0; 
            scanf("%d", &changeNum); //교환을 원하는 번호를 입력 받기

            //입력한 번호가 리스트 범위 안에 있을 때
            if(changeNum > 0 && changeNum <= nowListSize){
                int changeIndex = changeNum - 1; //입력한 번호를 실제 인덱스로 변경
                struct AddItem sendItem; //교환할 물건의 정보를 저장하는 구조체 선언
                //선택한 정보를 구조체에 저장하기
                strcpy(sendItem.have, allItemList[changeIndex].have); 
                strcpy(sendItem.want, allItemList[changeIndex].want);
                sendStruct(sd, sendItem, 1); //식별 번호 1번(교환 물건 신청)과 함께 교환 정보를 서버에게 전송
                printf("\n교환이 완료되었습니다.\n");
            }else{
                printf("입력된 번호가 범위 안에 없습니다.\n");
            }
        }else if (taskNum == 2){ //선택한 메뉴가 2번 "등록"일 경우
            createNew(sd); //등록 함수 호출
        }else if(taskNum == 3){ //선택한 메뉴가 3번 "나가기"일 경우
            printf("이용해주셔서 감사합니다. \n"); //인사말을 출력하고
            isRun = 0; //연속 실행 취소
        }else{ //입력한 번호가 틀릴 경우
            printf("잘못 입력했습니다. 정수 1, 2, 3중 하나를 입력하세요.\n ");
        }
        printf("\n");
    }

    //클라이언트 프로세스 종료 시
    struct AddItem empty; //빈 구조체를 만들고
    sendStruct(sd, empty, 3); //식별 번호 3번(클라이언트 종료)과 함께 서버에게 종료를 알림
    close(sd); //소켓 기술자 종료
}
