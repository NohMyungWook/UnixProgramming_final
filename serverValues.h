struct item{
    char want[50];
    char have[50];
};

struct user_input{
    int no;
    struct item item;
};

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};