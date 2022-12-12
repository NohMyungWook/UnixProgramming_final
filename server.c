#include <sys/shm.h>
#include <stdbool.h>

#include "serverValues.h"
#include "serverList.h"
#include "serverSemaphore.h"

//유닉스 도메인 소켓의 경로명
#define SOCKET_NAME "market"


int main(){
    //공유 메모리 관련 변수 선언
    int shmidList, shmidInt, i; //공유 메모리 식별자(교환 정보 리스트, 리스트 크기) 선언
    struct item *shmaddrList; //"교환 정보 리스트"의 공유 메모리 포인터 선언
    int* shmaddrInt; //"교환 정보 리스트 크기"의 공유 메모리 포인터 선언

    //교환 정보 리스트의 공유 메모리 식별자 선언
    shmidList = shmget(IPC_PRIVATE, sizeof(struct item) * (50), IPC_CREAT|0644);
    if (shmidList == -1) { //선언이 실패 시
        perror("shmget"); //오류 메시지 출력
        exit(1); //프로세스 종료
    }
    //교환 정보 리스트 크기의 공유 메모리 식별자 선언
    shmidInt = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT|0644);
    if (shmidInt == -1) { //선언이 실패 시
        perror("shmget"); //오류 메시지 출력
        exit(1); //프로세스 종료
    }

    //공유 메모리 연결
    shmaddrList = (struct item*)shmat(shmidList, (struct item*)NULL, 0);
    shmaddrInt = (int * )shmat(shmidInt, (int *)NULL, 0);
    
    //소켓 프로그래밍을 위함 변수 선언
    struct sockaddr_un ser, cli; //소켓 주소 구조체(서버, 클라이언트) 선언
    int sd, len, nsd, clen; //소켓 기술자, 소켓 주소 구조체 크기 선언

    struct item* itemsListPointer = shmaddrList; //교환 정보 리스트의 공유 메모리 주소 연결
    int* items_num_Pointer = shmaddrInt; //교환 정보 리스트 크기의 공유 메모리 주소 연결
    *items_num_Pointer = 0; //리스트 크기를 0로 초기화
    struct user_input user_input; //클라이언트 패킷 구조체 선언

    //기존의 유닉스 도메인 연결 중지
    unlink(SOCKET_NAME);
    //소켓 파일 기술자 생성
    if((sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }

    //test
    strcpy((itemsListPointer + *items_num_Pointer)->want, "want1");
    strcpy((itemsListPointer + *items_num_Pointer)->have, "have1");
    *items_num_Pointer += 1;

    strcpy((itemsListPointer + *items_num_Pointer)->want, "want2");
    strcpy((itemsListPointer + *items_num_Pointer)->have, "have2");
    *items_num_Pointer += 1;
    //end of test 

    //소켓 주소 구조체 초기화
    memset((char*)&ser, 0, sizeof(struct sockaddr_un));
    ser.sun_family = AF_UNIX; //유닉스 도메인 소켓 지정
    strcpy(ser.sun_path, SOCKET_NAME); //경로 지정
    len = sizeof(ser.sun_family) + strlen(ser.sun_path); //주고체 크기 계산

    //소켓 기술자를 지정된 유닉스 도메인 경로와 결합
    if(bind(sd, (struct sockaddr *)&ser, len)){
        perror("bind"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }

    //클라이언트의 연결 요청 대기
    if(listen(sd, 10) < 0){
        perror("listen"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    } 
    printf("Waiting for Client...\n"); //서버 로그 출력
    
    while(1){
        clen = sizeof(cli); //클라이언트의 소켓 주소 구조체 크기 계산
        //클라이언트의 연결 요청 수락
        if((nsd = accept(sd, (struct sockaddr *)&cli, &clen)) == -1){
            perror("accept"); //실패 시 오류 메시지 출력
            exit(1); //프로세스 종료
        }
        switch (fork()){ //새 클라이언트를 위한 자식 프로세스를 생성
            case -1: // 자식 프로세스 생성 실패 시
                perror("fork"); //오류 메시지 출력
                exit(1); //프로세스 종료
            case 0: // 자식 프로세스일 생성 성공 시
                //공유 메모리 주소 연결
                shmaddrList = (struct item*)shmat(shmidList, (struct item*)NULL, 0); //교환 정보 리스트의 공유 메모리 주소 연결
                itemsListPointer = shmaddrList; //공유 메모리 주소 전달
                shmaddrInt = (int * )shmat(shmidInt, (int *)NULL, 0); //교환 정보 리스트 크기의 공유 메모리 주소 연결
                items_num_Pointer = shmaddrInt; //공유 메모리 주소 전달
                int semid = initsem(1); //세마포어 초기화
                if(semid < 0)  //세마포어 초기화 실패 시
                    exit(1); //프로세스 종료
                char client_pid[50]; //클라이언트 PID를 저장할 문자열 선언
                printf("New Client connect!\n");
                //서버 자식 프로세스 초기 설정 완료//

                while(1){ //서비스 제공 시작
                    //클라이언트에서 보내온 패킷을 상시 수신
                    if(recv(nsd, (struct user_input*)&user_input, sizeof(user_input), 0) == -1){
                        perror("recv"); //실패 시 오류 메시지 출력
                        exit(1); //프로세스 종료
                    }
                    else{ //수신 성공 시
                        switch(user_input.no){ //수신 패킷의 식별 번호에 따라
                            case 0: //식변 번호 = 0: 교환 정보 리스트를 클라이언트에게 전송
                                send_item_list(nsd, (*items_num_Pointer), itemsListPointer, (int)getpid());
                                break;
                            case 1: //식별 번호 = 1: 물건 교환 요청 
                                semlock(semid); //임계 영역 접근 시도

                                //교환 정보 리스트 안에서 
                                for(int i=0; i<*items_num_Pointer; i++){
                                    // 클라이언트가 보내온 정보와 같은 정보가 있을 때
                                    if((strcmp((itemsListPointer + i)->want, user_input.item.want) + strcmp((itemsListPointer + i)->have, user_input.item.have))==0){
                                        printf("Delete List: index %d\n",i+1); //서버 로그 출력
                                        delete_item(itemsListPointer, i, *items_num_Pointer); //해당 교환 정보 삭제
                                        *items_num_Pointer -=1; //교환 정보 리스트 크기가 1씩 감소
                                    }
                                }
                                semunlock(semid); //임계 영역 접근 종료
                                break;
                            case 2: //식별 번호 = 2: 교환 물건 등록
                                semlock(semid); //임계 영역 접근 시도

                                //교환 물건 등록 성공 시
                                if(add_item(&user_input, itemsListPointer, *items_num_Pointer)){
                                    *items_num_Pointer += 1; //교환 정보 리스트 크기가 1씩 증가
                                    printf("Add List: index %d\n", *items_num_Pointer); //서버 로그 출력
                                }

                                semunlock(semid); //임계 영역 접근 시도
                                break;
                            case 3: //식별 번호 = 3: 클라이언트 종료
                                printf("Client disconnect! # of Proc: %d\n\n", (int)getpid());
                                close(nsd); //해당 소켓 기술자 닫음
                                exit(1); //프로세스 종료
                        }
                    }
                }  
        }
    }
    close(nsd); //클라이언트 소켓 기술자 닫음 
    close(sd); //서버 소켓 기술자 닫음
}