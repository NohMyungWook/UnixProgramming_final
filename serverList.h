#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>

void send_item_list(int nsd, int items_num, struct item *items, int pidno){
    printf("Send Item list to %d\n\n", pidno);
    send(nsd, &items_num, sizeof(items_num), 0);
    for(int i=0; i<items_num; i++){
        if(send(nsd, (struct item *)(items + i), sizeof(struct item), 0)== -1){
            perror("send");
            exit(1);
        }
    }
    return;
}

bool add_item(struct user_input *user_input, struct item *items, int items_num){
    if(items_num > 50){
        printf("Cannot add item\n");
        return false;
    }
    strcpy((items + items_num)->have, user_input->item.have);
    strcpy((items + items_num)->want, user_input->item.want);
    return true;
}

void delete_item(struct item *items, int n, int items_num){
    for(int i=n; i<items_num; i++){
        memcpy((items + i), (items + i+1), sizeof(struct item));
    }
    return;
}




