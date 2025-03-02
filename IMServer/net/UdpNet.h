#pragma once
#include"INet.h"
class UdpNet :public INet
{

public:
	UdpNet(INetMediator* pMediator);
	~UdpNet();
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UninitNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
protected://����ɱ����ģ�����û�е�������
	//��������
	void RecvData();
	static unsigned _stdcall RecvThread(void* lpVoid);//����Ϊ��̬�ģ���Ϊ�������Ҫ��Ϊ�����߳�_beginthreadex�����Ĳ���������ͨ��Ա����Ҫͨ���������ܵ���
	//���Զ���Ϊ��̬�ģ�����ͨ���������
	HANDLE m_handle;
	SOCKET m_sock;
	bool m_isStop;
};