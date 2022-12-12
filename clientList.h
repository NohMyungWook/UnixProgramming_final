//교환 정보 리스트 크기를 패킷으로 받고 반환하는 함수
int getCount(int sd);

//교환 정보 리스트를 받고 해당 정보를 저장 및 출력
void get1Item(int sd, int count, struct AddItem* allItemList);

//교환 정보 리스트를 출력 및 저장를 관리하는 함수
int printNowList(int sd, struct AddItem* allItemList);