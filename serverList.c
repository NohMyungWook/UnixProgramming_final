#include <sys/socket.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "serverValues.h"

//교환 정보 리스트를 클라이언트에게 보내는 함수
void send_item_list(int nsd, int items_num, struct item *items, int pidno){
    printf("%d : Send Item list to client\n\n", pidno); //현재 동작하는 서버의 pid 함께 로그 출력
    send(nsd, &items_num, sizeof(items_num), 0); //클라이언트에게 리스트 사이즈를 전송
    for(int i=0; i<items_num; i++){ //리스트 안의 각 교환 정보에 대해
        //교환 정보 구조체를 클라이언트에게 전송
        if(send(nsd, (struct item *)(items + i), sizeof(struct item), 0)== -1){
            perror("send"); //실패 시 오류 메시지 출력
            exit(1); //프로세스 종료
        }
    }
    return;
}

//교환 정보를 새로 추가하는 함수
bool add_item(struct user_input *user_input, struct item *items, int items_num){
    if(items_num > 50){ //교환 정보의 개수가 50을 초과할 때
        printf("Cannot add item\n"); // 서버 로그 출력
        return false; //false 리턴
    }
    //교환 정보를 리스트에 추가하기
    strcpy((items + items_num)->have, user_input->item.have);
    strcpy((items + items_num)->want, user_input->item.want);
    return true; //true 리턴
}

//교환 정보를 삭제하는 함수
void delete_item(struct item *items, int n, int items_num){
    for(int i=n; i<items_num; i++){ //삭제할 구조체부터의 각 교환 정보에 대해
        //메모리 복사를 통해 삭제할 구조체 뒤의 구조체들이 앞으로 한 칸 씩 옮김.
        memcpy((items + i), (items + i+1), sizeof(struct item)); 
    }
    return;
}




