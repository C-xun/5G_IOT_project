#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <mosquitto.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct Data {
	float Temps;
	float Humis;
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
	printf("L%d:rc = %d\n",__LINE__,rc);
}
// 断开连接回调函数，在断开连接之后进入
void my_disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
	printf("L%d:rc = %d\n",__LINE__,result);
}
// 消息回调
void my_message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	time_t t;
	struct tm *lt;
	time(&t);
	lt =  localtime(&t);
	printf("%d.%d.%d %d:%d:%d ",lt->tm_year+1900,lt->tm_mon,lt->tm_mday,lt->tm_hour,lt->tm_min,lt->tm_sec);
	printf("%d:topic(%s)->%s\n",__LINE__,(char *)msg->topic,(char *)msg->payload);	
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
void *mosquitto_wait(void *data)
{
	char *topic1 = "CONTECT";
	struct mosquitto *m_hMqtt = data;
	
	//设置连接确认回调,服务于mosquitto_connect()
	mosquitto_connect_callback_set(m_hMqtt, my_connect_callback);
 	//设置与服务器断开连接的回调
    mosquitto_disconnect_callback_set(m_hMqtt, my_disconnect_callback);
	//设置确认订阅回调(mosquitto_subscribe()订阅后回调)
	mosquitto_subscribe_callback_set(m_hMqtt, my_subscribe_callback);
	//设置收到订阅主题的发布内容时的回调
	mosquitto_message_callback_set(m_hMqtt, my_message_callback);
	//开始连接服务器
	if(MOSQ_ERR_SUCCESS == mosquitto_connect(m_hMqtt, "10.30.155.24", 1883, 2))
		puts("连接服务器成功");
	else{
		puts("连接服务器失败");
		mosquitto_destroy(m_hMqtt);
		mosquitto_lib_cleanup();
		return NULL;
	}
	mosquitto_subscribe(m_hMqtt, NULL, topic1, 1);
	mosquitto_loop_start(m_hMqtt);	
	/* 阻塞等待 */
	mosquitto_loop_stop(m_hMqtt, false);
	return NULL;
}
int main(int argc,char *argv[])
{
	int fd = 0,ret = 0;
	int temp,humi;
	char addr[8] = "";
	struct mosquitto *m_hMqtt;
	char *topic1 = "CONTECT";
	char content[1024] = "";
	pthread_t tid;

	fd = open("/dev/ttyUSB0",O_RDWR);
	if(fd < 0){
		perror("/dev/ttyUSB0");
		return -1;
	}

	//初始化lib库函数
	mosquitto_lib_init();
    // 定义一个客户端名为subtest的发布端。客户端表示订阅端的唯一性
    m_hMqtt = mosquitto_new("pubdemo", true, "data");
	pthread_create(&tid,NULL,mosquitto_wait,(void *)m_hMqtt);
	
	while(1){
		ret = read(fd, content,sizeof(content));
		if (ret < 0)
		{
			perror("read");
			return -1;
		}
		mosquitto_publish(m_hMqtt,NULL,topic1,strlen(content)+1,content,1,true);	
		//usleep(1000*1000);
		bzero(content, sizeof(content));
	}
	mosquitto_destroy(m_hMqtt);
    mosquitto_lib_cleanup();
	return 0;
}