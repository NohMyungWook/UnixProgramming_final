//교환 정보 리스트를 클라이언트에게 보내는 함수
void send_item_list(int nsd, int items_num, struct item *items, int pidno);

//교환 정보를 새로 추가하는 함수
bool add_item(struct user_input *user_input, struct item *items, int items_num);

//교환 정보를 삭제하는 함수
void delete_item(struct item *items, int n, int items_num);