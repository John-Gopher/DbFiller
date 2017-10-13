#include <stdio.h>
#include <mysql.h>
#include <string.h>

void main(void)
{
    MYSQL *t_mysql;
    char create[] = "create table stu(name varchar(30),stunum int)"; 
    char insert[] = "insert into stu values('chris', 3306)";
    char select[] = "select * from stu";
    t_mysql = mysql_init(NULL);
    if(NULL == t_mysql)
        printf("init failed\n");
    if(NULL == mysql_real_connect(t_mysql, "127.0.0.1", "root", "root", "", 3306, NULL, 128))
        printf("connect failed\n");

/*    if(mysql_real_query(t_mysql, create, strlen(create)) != 0)
        printf("create failed\n");

    if(mysql_real_query(t_mysql, insert, strlen(insert)) != 0)
        printf("insert failed\n");
*/

}

