#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "mosquitto.h"
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

typedef struct Data {
	int Temps;
	int Humis;
}Datas;
typedef struct _msg {
	long mType;
	char mName[16];
	Datas data;
}MSG;
long type[] = { 10,11 };
char *name_tmp[] = { "Temps", "Humis" };
int n = sizeof(name_tmp) / sizeof(name_tmp[0]);
// 连接回调函数，当连接成功时会进入这里，可以在这里进行连接成功之后的操作，比如连接之后的信息同步
void my_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
}
// 断开连接回调函数，在断开连接之后进入
void my_disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("%d:\n",__LINE__);
}
// 消息回调
void my_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	time_t t;
	struct tm *lt;
	time(&t);
	lt =  localtime(&t);
	printf("%d.%d.%d %d:%d:%d ",lt->tm_year+1900,lt->tm_mon+1,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	printf("%d:topic(%s)->%s\n",__LINE__,(char *)msg->topic,(char *)msg->payload);	

	key_t key = ftok("/", 1201);

	//创建一个消息队列
	int msg_id = msgget(key, IPC_CREAT | 0666);

	//发送消息：给谁发  这个谁 是由消息类型决定的
	MSG _msg;
	if (strstr((char *)msg->payload, "Temp is:") != NULL)
	{
		int temp;
		char content[1024] = "";
		strcpy(content, (char *)msg->payload);

		sscanf(content, "Temp is:%d\n", &temp);
		_msg.mType = type[0];
		strcpy(_msg.mName, name_tmp[0]);
		_msg.data.Temps = temp;
		
	}
	else if (strstr((char *)msg->payload, "Humidity is") != NULL)
	{
		int humi;
		char content[1024] = "";
		strcpy(content, (char *)msg->payload);

		sscanf(content, "Humidity is:%d\n", &humi);
		_msg.mType = type[1];
		strcpy(_msg.mName, name_tmp[1]);
		_msg.data.Humis = humi;
	}
	//发送消息：
	msgsnd(msg_id, &_msg, sizeof(_msg) - sizeof(long), 0);
}
// 订阅消息回调
void my_subscribe_callback(struct mosquitto *mosq, void *obj, int mid,int qos_count,const int *granted_qos)
{
	int i;
	time_t t;
	struct tm *lt;
	time(&t);
	lt =  localtime(&t);
	printf("%d: %s\n",__LINE__,(char *)obj);
	printf("%d: mid=%d\n",__LINE__,mid);
	printf("%d: qos_count=%d\n",__LINE__,qos_count);
	for(i=0;i<qos_count;i++)
		printf("%d: granted_qos[%d]=%d\n",__LINE__,i,granted_qos[i]);
}
int main(int argc,char *argv[])
{
	struct mosquitto *m_hMqtt;
	char *topic1 = "CONTECT";
	
	//初始化lib库函数
	mosquitto_lib_init();
    // 定义一个客户端名为subtest的发布端。客户端表示订阅端的唯一性
    m_hMqtt = mosquitto_new("subdemo", true, "data");
	
	mosquitto_connect_callback_set(m_hMqtt, my_connect_callback);
    mosquitto_disconnect_callback_set(m_hMqtt, my_disconnect_callback);
	//订阅回调
	mosquitto_message_callback_set(m_hMqtt, my_message_callback);
	mosquitto_subscribe_callback_set(m_hMqtt, my_subscribe_callback);
	mosquitto_connect(m_hMqtt, "10.30.155.24", 1883, 2);
	mosquitto_subscribe(m_hMqtt,NULL,topic1,1);
	mosquitto_loop_start(m_hMqtt);
	/* 阻塞等待 */
	mosquitto_loop_stop(m_hMqtt, false);
	
	mosquitto_destroy(m_hMqtt);
    mosquitto_lib_cleanup();
	return 0;
}