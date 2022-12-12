// #include <sys/socket.h>
// #include <sys/un.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <signal.h>

int sendStruct(int clientSD, struct AddItem newItem, int no){
	//보낼 패킷을 만들기
    struct SendPacket newPacket;
    newPacket.no = no;
    newPacket.nowItem = newItem;
    
    if (send(clientSD, (struct SendPacket*) &newPacket, sizeof(newPacket), 0) == -1) {
        perror("send");
        return 0;
    }
	return 1;
}


int createNew(int sd){
    struct AddItem newItem;
    
    printf("원하는 물건을 입력하세요 : ");
    scanf("%s", newItem.want);

    printf("가지고 있는 물건을 입력하세요 : ");
    scanf("%s", newItem.have);

    sendStruct(sd, newItem, 2);

    printf("\n등록되었습니다. \n");
    return 1;
}
