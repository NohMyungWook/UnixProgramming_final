// #include <sys/socket.h>
// #include <sys/un.h>
// #include <stdlib.h>
// #include <stdio.h>

int getCount(int sd){
    int nowCount;
    if(recv(sd, (int* )&nowCount, sizeof(nowCount), 0) == -1){
        perror("Count Recv");
        exit(1);
    }
    else{
        return nowCount;
    }
    return -1;
}

int get1Item(int sd, int count, struct AddItem* allItemList){
    
    for (int i = 0;i< count; i++){
        struct AddItem newItem;
        memset((struct AddItem *)&newItem, '\0', sizeof(struct AddItem));

        if(recv(sd, (struct AddItem * )&newItem, sizeof(struct AddItem), 0) == -1){
        perror("recv");
        exit(1);
        }
        else{
            *(allItemList + i) = newItem;
            printf("[%d]\n", i+1);
            printf("원하는 물건: %s\n", newItem.want);
            printf("갖고 싶은 물건: %s\n", newItem.have);
        }
    }
}

int printNowList(int sd, struct AddItem* allItemList){
    struct AddItem empty;
    sendStruct(sd, empty, 0);

    printf("-----현재 물물교환 가능 리스트-----\n");

    int itemInt = getCount(sd);

    if (itemInt > 0){
        get1Item(sd, itemInt, allItemList);
    }else{
        printf("현재 물물교환이 가능한 물건이 없습니다..\n\n");
    }

    printf("---------------------------------------\n");
    return itemInt;
}
