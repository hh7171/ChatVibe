#pragma once
#include"INet.h"
#include <map>
#include <list>
class TcpServerNet :public INet
{

public:
	TcpServerNet(INetMediator* pMediator);
	~TcpServerNet();
	//初始化网络
	bool InitNet();
	//关闭网络
	void UninitNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
protected://定义成保护的，外面没有调用它的
	//接收数据
	void RecvData();
	//接收数据的线程
	static unsigned _stdcall RecvThread(void* lpVoid);//定义为静态的，因为这个函数要作为创建线程_beginthreadex函数的参数，但普通成员函数要通过类对象才能调用
	//接收连接的线程
	static unsigned _stdcall AcceptThread(void* lpVoid);
	//所以定义为静态的，不用通过对象调用
	SOCKET m_sockListen;
	bool m_isStop;
	//定义一个map，key 是线程id，value 是客户端对应的socket
	map<unsigned int, SOCKET> m_mapThredIdToSocket;
	//定义一个链表，存储线程的句柄（方便增加删除操作）
	list<HANDLE>m_listThreadHandle;
};