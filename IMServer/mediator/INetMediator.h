#pragma once
//#include "INet.h"
class INet;
class INetMediator {

public:
	INetMediator() {}
	virtual ~INetMediator() {} //ʹ��������������ָ��ָ���������������ʱ����ִ��������������ִ�и�����������ֹ�ڴ�й©
	//������
	virtual bool OpenNet()=0;
	//�ر�����
	virtual void CloseNet()=0;
	//��������
	virtual bool SendData(long lSendIp, char* buf, int nLen)=0;
	//��������
	virtual void DealData(long lSendIp, char* buf, int nLen)=0;
protected:
	INet* m_pNet;
};