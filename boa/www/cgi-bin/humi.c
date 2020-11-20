#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
typedef struct Data {
	int Temps;
	int Humis;
}Datas;
typedef struct _msg {
	long mType;
	char mName[16];
	Datas data;
}MSG;
int main(int argc, char const *argv[])
{
	printf("Content-type:text/html\n\n");

	key_t key = ftok("/", 1999);

	int msg_id = msgget(key, IPC_CREAT | 0666);

	MSG msg;
	Datas data;
	memset(&msg, 0, sizeof(msg));

	msgrcv(msg_id, &msg, sizeof(msg) - sizeof(long), 11, IPC_NOWAIT);

	printf("%d\n",msg.data.Humis);


	return 0;
}