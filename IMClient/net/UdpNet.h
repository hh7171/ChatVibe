#pragma once
#include"INet.h"
class UdpNet :public INet
{

public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//初始化网络
	bool InitNet();
	//关闭网络
	void UninitNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
protected://定义成保护的，外面没有调用它的
	//接收数据
	void RecvData();
	static unsigned _stdcall RecvThread(void* lpVoid);//定义为静态的，因为这个函数要作为创建线程_beginthreadex函数的参数，但普通成员函数要通过类对象才能调用
	//所以定义为静态的，不用通过对象调用
	HANDLE m_handle;
	SOCKET m_sock;
	bool m_isStop;
};