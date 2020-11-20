#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sqlite3.h"

#define MSG_PATH					"./"
#define MSG_PROID					1210
#define MSG_CGI_TO_SERVER_TYPE		100
#define MSG_SERVER_TO_CGI_TYPE		200


typedef struct _msg
{
	long mtype;
	char sub_msg[512];

}MSGQUE;

/*********************************************************************
*功能:	URL的解码
*参数:	result: URL的解码结果；
		url_src：URL的编码
*返回值:URL的解码结果地址
*********************************************************************/
char *url_decode(char *result, const char *url_src)
{
	const char *src = url_src;
	char *dst = result;
	while(*src)
	{
		if(*src == '%')
		{
			char tmp[3];
			tmp[0] = src[1];
			tmp[1] = src[2];
			tmp[2] = 0;
			*dst = strtoul(tmp, NULL, 16);
			src += 2;
		}
		else if(*src=='+')
		{
			*dst=strtoul("20",NULL,16);
		}
		else
			{
				*dst = *src;
			}
		dst++;
		src++;
	}
	*dst = 0;
	return result;
}
/*******************************************************************
				 ******功能:返回网页一个图表******
*******************************************************************/
int snd_xml(char *xml_name)
{
	int fd = open(xml_name,O_RDONLY);
	if(fd < 0)
	{
		printf("%s","<INFO></INFO>");
	}
	else
	{
		struct stat ptr;
		stat(xml_name,&ptr);
		char *buf=malloc(ptr.st_size+1);//临时非配一个空间，因为XML必须一次性读取完
		read(fd, buf, ptr.st_size+1);
		printf("%s",buf);
		free(buf);
	}
	return -1;
}
/*******************************************************************
				******功能:根据main返回值处理网页效果******
*******************************************************************/
int deal_server_result(char *data)
{
	int type = -1;
	char other[100] = {0};
	
	if(NULL == data)
	{
		printf("Content-Type:text/html\n\n");
		printf("-1:-1:-1\n");
	}
	else
	{
		sscanf(data, "%d:%s", &type,other);
		if(1 == type)
		{
			printf("Content-Type:text/xml\n\n");
		}
		else
		{
			printf("Content-Type:text/html\n\n");
		}
		switch(type)
		{
			case 1:
			{
				snd_xml(other);//snd xml
			}
			break;
			default:
			{
				printf("-1:-1:-1\n");
			}
			break;
		}
	}
	return -1;
}
/*********************************************************************
*功能: 创建消息队列，与main交流
*参数:	snd_data:   
		rcv_data：
*返回值：  1
*********************************************************************/
int communicate_with_main(char *snd_data, char *rcv_data)
{
	key_t key;
	int msgid;
	MSGQUE msg_snd;
	MSGQUE msg_rcv;

	key = ftok(MSG_PATH, MSG_PROID);
	msgid = msgget(key, IPC_CREAT | 0777);
	msg_snd.mtype = MSG_CGI_TO_SERVER_TYPE;
	strcpy(msg_snd.sub_msg, snd_data);
	msgsnd(msgid, &msg_snd, sizeof(MSGQUE)-sizeof(msg_snd.mtype), 0);
	msgrcv(msgid, &msg_rcv, sizeof(MSGQUE)-sizeof(msg_rcv.mtype), MSG_SERVER_TO_CGI_TYPE, 0);
	strcpy(rcv_data, msg_rcv.sub_msg);
	return 1;
}

int main(void)
{
	char *data = getenv("QUERY_STRING");
	
	if(NULL != data)
	{
		char *data_tmp = NULL;
		
		data_tmp = malloc(strlen(data));
	
		if(NULL != data_tmp)
		{
			char rcv_data[512] = {0};
			
			url_decode(data_tmp, data);
			
			communicate_with_main(data_tmp, rcv_data);

			deal_server_result(rcv_data);
		}
	}
	else 
	{
		;
	}
	return 0;
}
