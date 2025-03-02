#pragma once
#include <iostream>
#include "CMySql.h"  //���ݿ�
#include "INetMediator.h"
#include "packDef.h"
#include <map>
using namespace std;

//���庯��ָ��
class CKernel;
typedef void(CKernel::* pfun)(long, char*, int);
class CKernel
{
public:
	CKernel();
	~CKernel();
	bool startServer();//�򿪷�����
	void closeServer();//�رշ�����
	//��ʼ��Э������
	void setProtocolMap();

	//��ѯ��ǰ��¼�û��ĺ�����Ϣ�������Լ�����Ϣ��
	void getUserList(int userId);

	//�����û�id��ѯ�û���Ϣ(userInfo���������)    ��ȡ��һ�������������û������Լ�id���Լ���Ϣ���Ͳ������Ϣ 
	void getUserInfoById(STRU_FRIEND_INFO* userInfo, int userId);
	//���������յ�������
	void dealData(long lSendIp, char* buf, int nLen);

	//����ע������
	void dealRegisterRq(long lSendIp, char* buf, int nLen);

	//�����¼����
	void dealLoginRq(long lSendIp, char* buf, int nLen);

	//������������
	void dealChatRq(long lSendIp, char* buf, int nLen);

	//������Ӻ�������
	void dealAddFriendRq(long lSendIp, char* buf, int nLen);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(long lSendIp, char* buf, int nLen);

	//������������
	void dealOfflineRq(long lSendIp, char* buf, int nLen);

private:
	INetMediator* m_pMediator;//�н��߸���ָ�룬����ָ��TCP�н���Ҳ����ָ��UDP�н���
	CMySql sql;
	//��������飬�±���Э��ͷ����ģ�����������Ǻ���ָ��
	pfun m_netProtocolMap[_DEF_TCP_PROTOCOL_COUNT];
	//����һ��map��������ͻ��˶�Ӧ��socket
	map<int, SOCKET> m_mapIdToSocket;
public:
	static CKernel* pKernel;//��̬��Ա�������ʼ��
	//kernel���ж�����һ��mediator����ָ�룬ͨ��ָ����Ե���mediator�����Ա�����ͳ�Ա������������ʵ��mediator�����kernel���еĳ�Ա����������������ͬ�ķ�����
	// ��mediator���ж�����һ��kernel��ָ�룬�����ͻ��������������������Ĵ���
	//����Ϊ�˴ﵽ��mediator��ʹ��kernel���еĳ�Ա��Ŀ�ģ����ǿ�����kernel���ж���һ�������ľ�̬��Ա��kernel���ָ�룬���þ�̬��Ա����ȫ�ֳ�Ա��������mediator��ֱ��ʹ��kernel��ָ�����kernel���Ա��
};

