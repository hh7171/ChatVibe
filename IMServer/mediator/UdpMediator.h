#pragma once
#include"INetMediator.h"
class UdpMediator:public INetMediator {

public:
	UdpMediator();//ֻ������ʵ��
	~UdpMediator();//ʹ��������������ָ��ָ���������������ʱ����ִ��������������ִ�и�����������ֹ�ڴ�й©
	//������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen) ;
};