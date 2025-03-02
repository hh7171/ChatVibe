#pragma once
#include"INetMediator.h"
class UdpMediator:public INetMediator {

public:
	UdpMediator();//只声明不实现
	~UdpMediator();//使用虚析构，父类指针指向子类对象，析构的时候，先执行子类析构，再执行父类析构，防止内存泄漏
	//打开网络
	bool OpenNet();
	//关闭网络
	void CloseNet();
	//发送数据
	bool SendData(long lSendIp, char* buf, int nLen);
	//接收数据
	void DealData(long lSendIp, char* buf, int nLen) ;
};