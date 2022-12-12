#define SOCKET_NAME "market" //유닉스 도메인 소켓의 경로명

struct item{ //교환 정보를 저장할 구조체 정의 
    char want[50]; //원하는 물건
    char have[50]; //가지고 싶은 물건
};

struct user_input{ //식별 번호 및 교환 정보 구조체가 포함한 구조체 정의
    int no; //식별 번호
    struct item item; //교환 정보를 저장할 구조체
}; 

union semun{ //세마포어 공용체
    int val; 
    struct semid_ds *buf; 
    unsigned short *array;
};