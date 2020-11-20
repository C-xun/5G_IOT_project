#include <stdio.h>
#include <sqlite3.h>
int callback(void * para, int n_column, char **column_value, char **column_name) 
{ 
	int i=0; 
	for(i=0;i<n_column;i++) 
	{ 
		printf("| %s | ", column_value[i]); 
	}
	printf("\n");
	return 0; 
}
int main()
{
	sqlite3 *db;
	char *errmsg;
	int ret=sqlite3_open("IOT.db",&db);

	sqlite3_exec(db,"create view Content as select TEMP.id,time,temp,humi from TEMP,HUMI where TEMP.id=HUMI.id;",NULL,NULL,&errmsg);
	sqlite3_exec(db,"select * from Content order by id desc;",callback,NULL,&errmsg);

	ret=sqlite3_close(db);

	return 0;
}