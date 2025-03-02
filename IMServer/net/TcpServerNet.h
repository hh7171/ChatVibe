#pragma once
#include"INet.h"
#include <map>
#include <list>
class TcpServerNet :public INet
{

public:
	TcpServerNet(INetMediator* pMediator);
	~TcpServerNet();
	//��ʼ������
	bool InitNet();
	//�ر�����
	void UninitNet();
	//��������
	bool SendData(long lSendIp, char* buf, int nLen);
protected://����ɱ����ģ�����û�е�������
	//��������
	void RecvData();
	//�������ݵ��߳�
	static unsigned _stdcall RecvThread(void* lpVoid);//����Ϊ��̬�ģ���Ϊ�������Ҫ��Ϊ�����߳�_beginthreadex�����Ĳ���������ͨ��Ա����Ҫͨ���������ܵ���
	//�������ӵ��߳�
	static unsigned _stdcall AcceptThread(void* lpVoid);
	//���Զ���Ϊ��̬�ģ�����ͨ���������
	SOCKET m_sockListen;
	bool m_isStop;//����һ����־λ���Ƿ��˳�
	//����һ��map��key ���߳�id��value �ǿͻ��˶�Ӧ��socket���ͻ��ˣ�socket�����������߳���������һһ��Ӧ�Ĺ�ϵ���������߳�id��ΪΨһ��ʶ��
	map<unsigned int, SOCKET> m_mapThredIdToSocket;
	//����һ�������洢�̵߳ľ������������ɾ�������� ����Ϊ���ֻ�ڹر������ʱ���õ��������о����ɾ�������ǲ���Ҫ֪�����ĸ��������˳��ɾ����ֻ�ùر����о���Ϳ��ԣ�������list�洢)��ʹ�ö�̬������ڿռ��˷ѵ����⣬����ʱһ�������ݺܴ�
	//�ر���ĳ�˵����촰�ڲ���ĳ�������ˣ�ֻ����ʱ������������
	list<HANDLE>m_listThreadHandle;
};