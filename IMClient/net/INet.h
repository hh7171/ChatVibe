#pragma once
#include <winsock.h>
#include<iostream>
using namespace std;
#pragma comment (lib,"Ws2_32.lib")
//#include "INetMediator.h"//不通过加头文件的方式添加INetMediator类，这种方法，在INetMediator头文件中加上INet头文件之后会形成一个环
//通过声明的方式
class INetMediator;//意思是我先声明有这个类，在之后的编译中会用到
class INet {

public:
	INet() {}
	virtual ~INet(){} //使用虚析构，父类指针指向子类对象，析构的时候，先执行子类析构，再执行父类析构，防止内存泄漏
	//初始化网络
	virtual bool InitNet()=0;
	//关闭网络
	virtual void UninitNet()=0;
	//发送数据
	virtual bool SendData(long lSendIp,char* buf,int nLen)=0;
protected:
	//接收数据
	virtual void RecvData()=0;
	INetMediator* m_pMediator;
};