#pragma once
#include <iostream>
#include "CMySql.h"  //数据库
#include "INetMediator.h"
#include "packDef.h"
#include <map>
using namespace std;

//定义函数指针
class CKernel;
typedef void(CKernel::* pfun)(long, char*, int);
class CKernel
{
public:
	CKernel();
	~CKernel();
	bool startServer();//打开服务器
	void closeServer();//关闭服务器
	//初始化协议数组
	void setProtocolMap();

	//查询当前登录用户的好友信息（包括自己的信息）
	void getUserList(int userId);

	//根据用户id查询用户信息(userInfo是输出参数)    提取出一个函数，用于用户根据自己id查自己信息，和查好友信息 
	void getUserInfoById(STRU_FRIEND_INFO* userInfo, int userId);
	//处理所有收到的数据
	void dealData(long lSendIp, char* buf, int nLen);

	//处理注册请求
	void dealRegisterRq(long lSendIp, char* buf, int nLen);

	//处理登录请求
	void dealLoginRq(long lSendIp, char* buf, int nLen);

	//处理聊天请求
	void dealChatRq(long lSendIp, char* buf, int nLen);

	//处理添加好友请求
	void dealAddFriendRq(long lSendIp, char* buf, int nLen);

	//处理添加好友回复
	void dealAddFriendRs(long lSendIp, char* buf, int nLen);

	//处理下线请求
	void dealOfflineRq(long lSendIp, char* buf, int nLen);

private:
	INetMediator* m_pMediator;//中介者父类指针，可以指向TCP中介者也可以指向UDP中介者
	CMySql sql;
	//定义个数组，下标是协议头计算的，数组的内容是函数指针
	pfun m_netProtocolMap[_DEF_TCP_PROTOCOL_COUNT];
	//定义一个map用来管理客户端对应的socket
	map<int, SOCKET> m_mapIdToSocket;
public:
	static CKernel* pKernel;//静态成员在类外初始化
	//kernel类中定义了一个mediator父类指针，通过指针可以调用mediator子类成员变量和成员函数，但是想实现mediator类调用kernel类中的成员函数，不能再用相同的方法，
	// 在mediator类中定义了一个kernel类指针，这样就会存在你中有我我中有你的错误。
	//所以为了达到在mediator类使用kernel类中的成员的目的，我们可以在kernel类中定义一个公共的静态成员，kernel类的指针，利用静态成员类似全局成员的性质在mediator类直接使用kernel类指针调用kernel类成员。
};

