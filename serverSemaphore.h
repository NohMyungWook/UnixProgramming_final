#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/sem.h>

int initsem(key_t semkey){
    union semun semunarg;
    int status=0, semid;
    semid = semget(semkey, 1, IPC_CREAT|IPC_EXCL|0600);
    if(semid == -1){
        if(errno == EEXIST)
            semid = semget(semkey, 1, 0);
    }
    else{
        semunarg.val = 1;
        status = semctl(semid, 0, SETVAL, semunarg);
    }

    if(semid == -1 || status == -1){
        perror("initsem");
        return (-1);
    }
    printf("sem_id: %d\n", semid);
    return semid;
}

int semlock(int semid){
    struct sembuf buf;
    pid_t pid = getpid();
    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    if(semop(semid, &buf, 1) == -1){
        perror("semlock failed");
        exit(1);
    }
    printf("------------------\n");
    printf("Process %d lock\n", (int)pid);
    return 0;
}

int semunlock(int semid){
    struct sembuf buf;
    pid_t pid = getpid();
    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    if(semop(semid, &buf, 1) == -1){
        perror("semunlock failed");
        exit(1);
    }
    printf("Process %d unlock\n", (int)pid);
    printf("------------------\n");
    return 0;
}

