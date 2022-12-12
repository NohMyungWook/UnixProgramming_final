#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>

#include "serverValues.h"

//세마포어 초기화 함수
int initsem(key_t semkey){
    //세마포어 관련 변수 선언
    union semun semunarg; //세마포어 공용체 선언
    int status=0, semid; //세마포어 값, 세마포어 ID 선언
    //세마포어 식별자 생성
    semid = semget(semkey, 1, IPC_CREAT|IPC_EXCL|0600);
    if(semid == -1){ //세마포어 식별자 생성 실패 시
        if(errno == EEXIST) //오류값이 EEXIST 일 때
            semid = semget(semkey, 1, 0); //기존 세마포어 사용
    }
    else{ //세마포어 식별자 생성 성공 시
        semunarg.val = 1; //세마포어 값을 1로 초기화
        status = semctl(semid, 0, SETVAL, semunarg); //세마포어 초기화
    }
    //세마포어 초기화 실패 시 
    if(semid == -1 || status == -1){
        perror("initsem"); //오류 메시지 출력
        return (-1); //-1 리턴
    }
    printf("sem_id: %d\n", semid); //세마포어 식별자 로그 출력
    return semid; //세마포어 식별자 리턴
}

//세마포어 잠금 시도 함수
int semlock(int semid){
    struct sembuf buf; //세마포어 제어 구조체 선언
    buf.sem_num = 0; //세마포어 번호 지정
    buf.sem_op = -1; //세마포어 연산 지정
    buf.sem_flg = SEM_UNDO; //세마포어 제어 Flag 지정
    if(semop(semid, &buf, 1) == -1){ //세마포어 연산 수행
        perror("semlock failed"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }
    printf("------------------\n");
    pid_t pid = getpid(); //현재 자식 프로세스 PID 검색
    printf("Process %d lock\n", (int)pid); //자식 프로세스 PID와 함께 로그 출력
    return 0;
}

//세마포어 잠금 해제 함수
int semunlock(int semid){
    struct sembuf buf; //세마포어 제어 구조체 선언
    buf.sem_num = 0; //세마포어 번호 지정
    buf.sem_op = 1; //세마포어 연산 지정
    buf.sem_flg = SEM_UNDO; //세마포어 제어 Flag 지정
    if(semop(semid, &buf, 1) == -1){ //세마포어 연산 수행
        perror("semunlock failed"); //실패 시 오류 메시지 출력
        exit(1); //프로세스 종료
    }
    pid_t pid = getpid(); //현재 자식 프로세스 PID 검색
    printf("Process %d unlock\n", (int)pid); //자식 프로세스 PID와 함께 로그 출력
    printf("------------------\n");
    return 0;
}

