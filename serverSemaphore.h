//세마포어 초기화 함수
int initsem(key_t semkey);

//세마포어 잠금 시도 함수
int semlock(int semid);

//세마포어 잠금 해제 함수
int semunlock(int semid);