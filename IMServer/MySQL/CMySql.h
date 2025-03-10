﻿#pragma once
#include <list>
#include<string>
using namespace std;
#include "mysql.h"
#pragma comment(lib,"libmysql.lib")

class CMySql
{
public:
    CMySql(void); 
    ~CMySql(void);
public:                    //ip,用户名,密码，数据库，端口号
    bool  ConnectMySql(char *host,char *user,char *pass,char *db,short nport = 3306);
    void  DisConnect();
    //查询
    bool  SelectMySql(char* szSql,int nColumn ,list<string>& lstStr);//char* szSql  SQL语句,int nColumn   列的个数,list<string>& lstStr  一个引用，存放查询到的数据，以string类型存放
	//获得数据库中的表
    bool GetTables(char* szSql,list<string>& lstStr);
    //更新：删除、插入、修改
    bool  UpdateMySql(char* szSql);
 
private:
    MYSQL *m_sock;
	MYSQL_RES *m_results;   
	MYSQL_ROW m_record; 
   
};

