#pragma once
#include <QObject>
//#include "INet.h"
class INet;
class INetMediator:public QObject{
    Q_OBJECT
public:
    INetMediator() ;//QT中类的构造和析构函数的实现是不允许写在头文件中的，所以我们再建一个INetMediator.cpp，这里改成声明
    virtual ~INetMediator()  ;//使用虚析构，父类指针指向子类对象，析构的时候，先执行子类析构，再执行父类析构，防止内存泄漏
	//打开网络
	virtual bool OpenNet()=0;
	//关闭网络
	virtual void CloseNet()=0;
	//发送数据
	virtual bool SendData(long lSendIp, char* buf, int nLen)=0;
	//接收数据
	virtual void DealData(long lSendIp, char* buf, int nLen)=0;
protected:
	INet* m_pNet;
};
