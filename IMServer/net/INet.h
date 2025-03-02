#pragma once
#include <winsock.h>
#include<iostream>
using namespace std;
#pragma comment (lib,"Ws2_32.lib")
//#include "INetMediator.h"//��ͨ����ͷ�ļ��ķ�ʽ���INetMediator�࣬���ַ�������INetMediatorͷ�ļ��м���INetͷ�ļ�֮����γ�һ����
//ͨ�������ķ�ʽ
class INetMediator;//��˼����������������࣬��֮��ı����л��õ�
class INet {

public:
	INet() {}
	virtual ~INet(){} //ʹ��������������ָ��ָ���������������ʱ����ִ��������������ִ�и�����������ֹ�ڴ�й©
	//��ʼ������
	virtual bool InitNet()=0;
	//�ر�����
	virtual void UninitNet()=0;
	//��������
	virtual bool SendData(long lSendIp,char* buf,int nLen)=0;
protected:
	//��������
	virtual void RecvData()=0;
	INetMediator* m_pMediator;
};