#pragma once
#include"INetMediator.h"
class TcpClientMediator :public INetMediator {

public:
	TcpClientMediator();//ֻ������ʵ��
	~TcpClientMediator();//ʹ��������������ָ��ָ���������������ʱ����ִ��������������ִ�и�����������ֹ�ڴ�й©
	//������
	bool OpenNet();
	//�ر�����
	void CloseNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
	//��������
	void DealData(long lSendIp, char* buf, int nLen);
};