struct AddItem
{
	char want[50];
	char have[50];
};

struct SendPacket
{
    int no;
    struct AddItem nowItem;
};