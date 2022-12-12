//식별 번호와 함께 서버에게 구조체를 전송하는 함수 
int sendStruct(int clientSD, struct AddItem newItem, int no);

//교환 물건을 등록하고 서버에게 보내는 함수
int createNew(int sd);