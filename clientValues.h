struct AddItem //교환 정보를 저장할 구조체 정의 
{
	char want[50]; //원하는 물건
	char have[50]; //가지고 싶은 물건
};

struct SendPacket //식별 번호 및 교환 정보 구조체가 포함한 구조체 정의
{
    int no; //식별 번호
    struct AddItem nowItem; //교환 정보를 저장할 구조체
};