#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "sqlite3.h"

#define DB_NAME						"IOT.db"

#define QUERY_XML					"IOT.xml"

#define MSG_PATH					"./"
#define MSG_PROID					1210
#define MSG_CGI_TO_SERVER_TYPE		100
#define MSG_SERVER_TO_CGI_TYPE		200
typedef struct _msg
{
	long mtype;
	char sub_msg[512];

}MSGQUE;

void print_db_result(int row, int col, char *result[])
{
	int i, j, index;
	
	index = 0;
	for(i=0;i<row;i++)
	{
		for(j=0;j<col;j++)
		{
			printf("%s: %s\n",result[j],result[index]);
			index++;
		}
	}
}
int create_xml(char *xml_name, char *root_name, char *son_name, int son_num, int grandson_num, char *grandson_name[], char *grandson_content[])
{
	int i = 0;
	int j = 0;
	int index = 0;
	
    //定义文档和节点指针
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");//指定版本为1.0
    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST root_name);//创建一个节点root用root_node标示
    
	//配置根节点
    xmlDocSetRootElement(doc,root_node);//将root节点即root_node指定为根节点

    //创建一个儿子和孙子节点
	for(i = 0; i < son_num; i++)
	{
	    xmlNodePtr node = xmlNewNode(NULL, BAD_CAST son_name);
	    xmlAddChild(root_node,node);
		for(j = 0; j < grandson_num; j++)
		{
			xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST grandson_name[j]);
			xmlAddChild(node,grandson);
			xmlAddChild(grandson, xmlNewText(BAD_CAST grandson_content[index]));//将文字添加到节点中 类似xmlNewTextChild()
			index++;
		}
	}
    
    //存储xml文档
    int nRel = xmlSaveFile(xml_name,doc);//指定文件存储的名字
    if (nRel != -1)
    {
       printf("一个xml文档被创建\n");   
	}
    //释放文档内节点动态申请的内存
    xmlFreeDoc(doc);
    return 1;
}

char** db_get_table(char *db_name, char *sqlcmd, int *nRow, int *nColumn)
{
	sqlite3 *db = NULL;
	int result, ret = -1;
	char * errmsg = NULL;
	char **dbResult = NULL;
	result = sqlite3_open(db_name, &db);
	if(result != SQLITE_OK)	
	{		
		perror("open\n");
		exit(-1);
	}
	result = sqlite3_get_table(db, sqlcmd, &dbResult, nRow, nColumn, &errmsg);
	if(result == SQLITE_OK)
	{
		printf("*nRow = %d\n", *nRow);
		if(*nRow > 0)
		{
			ret = 0;
		}
	}
	sqlite3_close(db);
	printf("ret = %d\n", ret);
	return (ret==0?dbResult:NULL);

}
/*********************************************************************
			*功能:     		查询数据库
*********************************************************************/
int query_proc(char *cmd, char *ret_data)
{
	char method[50] = {0};
	int device_id = 0;
	char sqlcmd[200] = {0};
	int ret = -1;
	int row = 0, col = 0;
	char **result = NULL;

	sscanf(cmd, "%*[^:]:%[^:]:%d", method, &device_id);
	printf("method = %s :text = %d\n", method, device_id);
	sprintf(sqlcmd, "select * from Content order by id desc limit 0,\'%d\';",device_id);
	printf("sqlcmd = %s\n", sqlcmd);
	result = db_get_table(DB_NAME, sqlcmd, &row, &col);
	if(NULL != result)
	{
		print_db_result(row, col, result);
		remove(QUERY_XML);
		create_xml(QUERY_XML, "QUERY", "INFO", row, col, result, &(result[col]));
		sqlite3_free_table(result);
		strcpy(ret_data,QUERY_XML);
		printf(" 返回信息ret_data===%s\n",ret_data);
		ret = 1;
	}
	return ret;
}
int deal_cmd(char *cmd,char *ret_data)
{
	char type_tmp[10] = {0};
	int act = 0;
	sscanf(cmd, "%[^:]", type_tmp);
	act = atoi(type_tmp);
	printf("act====%d\n",act);
	switch(act)
	{
		case 1:
		{
			return query_proc(cmd, ret_data);
		}
		break;
		default:
		break;
	}
	return -1;
}


/*********************************************************************
*功能:     main 端与客户端cgi交流
*参数:	            无
*返回值:            无
*********************************************************************/

int main_communication_with_cgi(void)
{
	key_t key;
	int msgid;
	MSGQUE msg_snd;
	MSGQUE msg_rcv;
	int ret = -1;
	char ret_data[200] = {0};
	char rcv_data[512] = {0};

	key = ftok(MSG_PATH, MSG_PROID);
	msgid = msgget(key, IPC_CREAT | 0777);
	msg_snd.mtype = MSG_SERVER_TO_CGI_TYPE;
	printf("in  main_communication_with_cgi \n");
	
	//1、读取网页发送消息队列的消息
	msgrcv(msgid, &msg_rcv, sizeof(MSGQUE)-sizeof(msg_rcv.mtype), MSG_CGI_TO_SERVER_TYPE, 0);
	
	//2、根据网页发送过来的请求消息指令，发送相应数据给网页
	//2.1、刷新页面请求（遍历缓存链表，并将信息通过消息队列发送给cgi）
	strcpy(rcv_data, msg_rcv.sub_msg);
	//如果是查询，得到的信息为1:id:棚号
	printf("recv html rcv_data == %s\n",rcv_data);
	ret = deal_cmd(msg_rcv.sub_msg,ret_data);
	
	sprintf(msg_snd.sub_msg, "%d:%s",ret, ret_data);
	printf("&&&要发送的数据:%s&&&",msg_snd.sub_msg);
	msgsnd(msgid, &msg_snd, sizeof(MSGQUE)-sizeof(msg_snd.mtype), 0);

	return 1;
}


int main()
{
	while(1)
    {
        main_communication_with_cgi();
    }
	return 0;
}