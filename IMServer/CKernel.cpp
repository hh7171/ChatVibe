#include "CKernel.h"
#include "TcpServerMediator.h"
//#include <list>

//������������±�ĺ��滻��ʽ
#define NetProtocolMap(a) m_netProtocolMap[a - _DEF_PROTOCOL_BASE - 10]

//��̬��Ա���������ʼ��
CKernel* CKernel::pKernel = NULL;
CKernel::CKernel() {
	pKernel = this;//����ָ���ָ����CKernel��
	setProtocolMap();
}
CKernel::~CKernel() {
	closeServer();
}
//�򿪷�����:�����磬���������ݿ�
bool CKernel::startServer() {
	//1��������
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->OpenNet()) {
		cout << "������ʧ��" << endl;
		return false;
	}
	//2�����������ݿ�
	if (!sql.ConnectMySql("127.0.0.1","root","","im")) {//127.0.0.1���ػػ���ַ���û��������룬����
		cout << "�������ݿ�ʧ��" << endl;
		return false;
	}
	return true;
}

//�رշ�����
void CKernel::closeServer() {
	//������Դ
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = NULL;
	}

}
//��ʼ��Э������
void CKernel::setProtocolMap() {
	//1����ʼ������
	memset(m_netProtocolMap, 0, sizeof(pfun)*_DEF_TCP_PROTOCOL_COUNT);

	//2����Э��ͷ�ʹ��������Ѵ������ĵ�ַ���ڶ�Ӧ���±��
	NetProtocolMap(_DEF_PROTOCOL_TCP_REGISTER_RQ) = &CKernel::dealRegisterRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_LOGIN_RQ) = &CKernel::dealLoginRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_CHAT_RQ) = &CKernel::dealChatRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ) = &CKernel::dealAddFriendRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS) = &CKernel::dealAddFriendRs;
	NetProtocolMap(_DEF_PROTOCOL_TCP_OFFLINE_RQ) = &CKernel::dealOfflineRq;

}

//���������յ�������
void CKernel::dealData(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealData" << endl;
	//1��ȡ��Э��ͷ
	int type = *(int*)buf;
	//2������Э��ͷ�ж����ĸ��ṹ�壬�߶�Ӧ�Ĵ�������
	//2.1���ж�Э��ͷ�Ƿ�����Чֵ��Χ��
	if ((type - _DEF_PROTOCOL_BASE - 10) >= 0 &&
		(type - _DEF_PROTOCOL_BASE - 10) < (_DEF_TCP_PROTOCOL_COUNT)) {
		//2.2��ͨ�����������±꣬ȡ��������ַ
		pfun pf = NetProtocolMap(type);
		// 2.3�����ú���
		if (pf) {
			(this->*pf)(lSendIp, buf, nLen);
		}
		else {
			cout << "type:" << type<<endl;
		}
	}
	//3�����������ݣ����տռ�
	delete[] buf;

}

//����ע������
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealRegisterRq" << endl;
	//1���������char*��Ϊ��Ӧ�ṹ��
	STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)buf;//��Ϊʲô����Ϊָ��STRU_REGISTER_RQ*����ΪbufΪָ�룬ָ����ָ�����ǿת��
	cout << "tel:" << rq->tel << ",name:" << rq->name << ",password:" << rq->password<<endl;
	
	//2��У���û���������Ƿ�Ϸ���У�������ͻ�һ��
	// ����Ϊ�յ�����������������˷�ð�ķǷ��������Կͻ��˺ͷ���˶�����У�飩---�ͻ����Ѿ�д���ˣ�����û����д����������Ҫ��

	STRU_REGISTER_RS rs;
	//3�����ݵ绰�����ѯ���ݿ����Ƿ��Ѿ����������
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";//�洢SQL���
	sprintf_s(sqlBuf, "select tel from t_user where tel='%s';",rq->tel);//sprintf_s(),��sqlBuf��д��"..."  sql���
	if (!sql.SelectMySql(sqlBuf,1,resultList)) {//SelectMySql()��ѯ���ݿ⣬������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" <<sqlBuf<< endl;
		return;
	}
	//4��������ѯ���
	if (resultList.size() > 0) {
		//˵�����ݿ����Ѿ���������绰����ע������˺��ˣ�����ע��ظ�������
		rs.result = tel_is_exist;
	}
	else {
		//û��ʹ������绰����ע��
		//5�������ǳƲ�ѯ���ݿ����Ƿ��Ѿ��������
		sprintf_s(sqlBuf, "select name from t_user where name='%s';", rq->name);//��sqlBuf��д��"..."sql���
		if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
			cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
			return;
		}

		//6��������ѯ���
		if (resultList.size() > 0) {
			//˵�����ݿ����Ѿ���������ǳ�ע������˺��ˣ�����ע��ظ������� 
			rs.result = name_is_exist;
		}
		else {
			//�ֻ��ź��ǳƶ�û�б�ע���������ע��
			//7��������˵�ע����Ϣд�����ݿ�
			sprintf_s(sqlBuf, "insert into t_user (tel,name,password,icon,feeling) values('%s','%s','%s',0,'ɶҲûд');",
				rq->tel,rq->name, rq->password);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "�������ݿ�ʧ��, sql:" << sqlBuf << endl;
			}

			//8������ע��ظ�����ɹ�
			rs.result = register_success;
		}

	}
	
	//9���ظ��ͻ���ע��ظ�
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//�����¼����
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealLoginRq" << endl;
	//1���������char*��Ϊ��Ӧ�ṹ��
	STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)buf;

	//2��У���û���������Ƿ�Ϸ���У�������ͻ�һ��
	// ����Ϊ�յ�����������������˷�ð�ķǷ��������Կͻ��˺ͷ���˶�����У�飩---�ͻ����Ѿ�д���ˣ�����û����д����������Ҫ��

	//3������tel������
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id,password from t_user where tel='%s';", rq->tel);//ע������Ȳ�id�ڲ����룬��ôresultList�д洢��Ҳ�����˳��������ȡ��ʱ��ҲӦ�ð������˳��ȡ
	if (!sql.SelectMySql(sqlBuf, 2, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
		return;
	}
	STRU_LOGIN_RS rs;
	//4�������ѯ���Ϊ��
	if (resultList.size() ==0) {
		//�û������ڣ����õ�¼�ظ����Ϊʧ��
		rs.result = user_not_exist;
		//7���յ���¼�����Ժ󣬻ظ����ͻ���һ����¼�ظ�
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
	else {
		//5�������ѯ�����Ϊ�գ�ȡ���û�id������(�Ȳ�id,Ҳ��ȡid)
		int userId = atoi(resultList.front().c_str());//�ַ�������idת����int���͵�id
		resultList.pop_front();//����id����һ����������

		string pas = resultList.front();
		resultList.pop_front();

		//6�����û������������бȽ��Ƿ���ͬ
		if (0==strcmp(rq->password,pas.c_str())) {//rq->password��char* ������char����ıȽϷ�ʽ
			//������ͬ����¼�ɹ�
			rs.result = login_success;
			rs.userId = userId;//��¼�ɹ��������Լ���id
			//���浱ǰ��¼�ɹ��û���socket
			m_mapIdToSocket[userId] = lSendIp;
			//7���յ���¼�����Ժ󣬻ظ����ͻ���һ����¼�ظ�
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//��¼�ɹ�ֱ�Ӳ�ѯ��ǰ��¼�û��ĺ�����Ϣ�������Լ�����Ϣ��
			getUserList(userId);
		}
		else {
			//���벻��ͬ����¼ʧ��
			rs.result = password_error;
			//7���յ���¼�����Ժ󣬻ظ����ͻ���һ����¼�ظ�
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}
	}

	//7���յ���¼�����Ժ󣬻ظ����ͻ���һ����¼�ظ�
	//m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));   //����ֻ����������﷢һ�ε�¼�ظ�����Ϊ�ڿͻ��ˣ����յ���¼�ɹ���Ϣ��
	// �ж�Ӧ�Ĳ������ж��ǲ����Լ���,���ֻ�ڵ�¼�ɹ�Ҳ��ѯ������Ϣ֮��ظ���¼�ظ���������ʹ�ͻ����ж��ǲ����Լ��Ĳ���û�н��У�������Ҳ������ʾ��ȡ�����Լ�����Ϣ
}

//��ѯ��ǰ��¼�û��ĺ�����Ϣ�������Լ�����Ϣ���������Լ���Idͨ���Լ���id����Ϣ
void CKernel::getUserList(int userId) {
	//1�� ��ѯ�Լ�����Ϣ
	STRU_FRIEND_INFO userInfo;
	getUserInfoById(&userInfo, userId);//����userId��ѯ�����û�����Ϣ�������userInfo��

	//2����userId ȡ��socket,���Լ�����Ϣ�����ͻ��ˣ���Ҫ�õ�socket���ж��ٸ��ͻ��˾��ж��ٸ�socket�����Խ�socket��������,��map,һһ��Ӧ�����ڲ��ң�
	if (m_mapIdToSocket.find(userId) == m_mapIdToSocket.end()) {//û�ҵ�
		cout << "CKernel::getUserList��ȡsocketʧ�ܣ�userId="<<userId<<endl;
		return;
	}
	m_pMediator->SendData(m_mapIdToSocket[userId],(char*)&userInfo,sizeof(userInfo));

	//3���Ȳ������Щ����
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA='%d';",userId);
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
		return;
	}

	//4�����ݺ��ѵ�id�б���ѯ��ÿ�����ѵ���Ϣ
	int friendId = 0;
	STRU_FRIEND_INFO friendInfo;
	while (resultList.size() > 0) {
		//5��ȡ������id
		friendId = atoi(resultList.front().c_str());
		resultList.pop_front();

		//6�����ݺ���Id ��ѯ������Ϣ
		getUserInfoById(&friendInfo, friendId);

		//7���Ѻ�����Ϣ������¼�ͻ���
		m_pMediator->SendData(m_mapIdToSocket[userId], (char*)&friendInfo, sizeof(friendInfo));

		//8���жϺ����Ƿ�����   ��¼�ɹ���socket�����m_mapIdToSocket������ͨ��m_mapIdToSocket�ж��Ƿ�����
		if (m_mapIdToSocket.find(friendId) != m_mapIdToSocket.end()) {
			//9���������ߣ������ѷ��͵�¼�˵���Ϣ
			m_pMediator->SendData(m_mapIdToSocket[friendId], (char*)&userInfo, sizeof(userInfo));

		}
	}
}

//�����û�id��ѯ�û���Ϣ(userInfo���������)
void CKernel::getUserInfoById(STRU_FRIEND_INFO* userInfo, int userId) {
	//1�������û�id
	userInfo->userId = userId;


	//2�������û�״̬
	if (m_mapIdToSocket.find(userId) != m_mapIdToSocket.end()) {//����
		userInfo->state = status_online;
	}
	else {
		userInfo->state = status_offline;
	} 

	//3�������ݿ��ѯ�û���������Ϣ
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id='%d';", userId);
	if (!sql.SelectMySql(sqlBuf, 3, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
		return;
	}
	//4����ȡ��ѯ��������õ��ṹ����
	if (resultList.size() == 3) {
		//��ȡ����
		strcpy_s(userInfo->name, resultList.front().c_str());
		resultList.pop_front();
		//��ȡicon
		userInfo->iconId=atoi(resultList.front().c_str());
		resultList.pop_front();
		//���ȡǩ��
		strcpy_s(userInfo->feeling, resultList.front().c_str());
		resultList.pop_front();
	}
}

//�����������󣨿ͻ��˸����ѷ���Ϣ����ʱ��
void CKernel::dealChatRq(long lSendIp, char* buf, int nLen) {//lSendIp ������Ϣ���ĸ��˵�socket
	cout << "CKernel::dealChatRq" << endl;
	//1�����
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;
	cout << "userId:" << rq->userId << ",friend:" <<rq->friendId<< ",content:" << rq->content<<endl;

	//2���鿴�����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId)>0) {
		//�������ߣ��Ͱ���������ת��������
		m_pMediator->SendData(m_mapIdToSocket[rq->friendId], buf, nLen);
	}
	else {
		//���Ѳ����ߣ��ظ����Ͷ˺��Ѳ�����
		STRU_TCP_CHAT_RS rs;
		rs.result = send_fail;
		rs.userId = rq->userId;
		rs.friendId = rq->friendId;
		m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));//lSendIp�ղŸ��ҷ���Ϣ���Ǹ��˵�socket

	}

}

//������Ӻ�������
void CKernel::dealAddFriendRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealAddFriendRq" << endl;

	//1�����
	STRU_ADD_FRIEND_RQ* rq = (STRU_ADD_FRIEND_RQ*)buf;

	//2���鿴���ݿ�����û���������ӵ��û�
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id from t_user where name='%s';", rq->friendName);//�������Ƿ���������Ҫ�õ������ǳƣ������ڿͻ��˴�������ǳƵ�ʱ����Ҫת��
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
		return;
	}
	STRU_ADD_FRIEND_RS rs;
	if (resultList.size()>0) {
		//��������������û�,ȡ��������û���id
		int friendId = atoi(resultList.front().c_str());
		resultList.pop_front();
		// 3���鿴������û��Ƿ�����
		if (m_mapIdToSocket.count(friendId) >0) {
			//4�����������û����ߣ��Ͱ���Ӻ��ѵ�����ת����������û�
			m_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
		}
		else {
			//5�����������û������ߣ����ظ��ͻ�����Ӻ���ʧ�ܣ�ԭ������----TODO:������ԸĽ�Ϊ��������ʱ�ȱ���������Ϣ��������ʱ�ٷ���
			rs.result = user_offline;
			rs.friendId = friendId;
			strcpy_s(rs.friendName, rq->friendName);
			rs.userId = rq->userId;
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

		}
	}
	else {
		//6��û�����������û������ظ��ͻ�����Ӻ���ʧ�ܣ�ԭ���û�������
		rs.result = no_this_user;
		strcpy_s(rs.friendName, rq->friendName);
		rs.userId = rq->userId;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
}

//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealAddFriendRs" << endl;
	//1�����
	STRU_ADD_FRIEND_RS* rs = (STRU_ADD_FRIEND_RS*)buf;

	//2������Է�ͬ����Ӻ��ѣ��ͰѺ��ѹ�ϵд�����ݿ⣨д�����Σ�Ϊ�˱�֤���ɹ�����ʧ�ܣ���Ҫʹ������
	if (add_success == rs->result) {
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "insert into t_friend values ('%d','%d');", rs->userId, rs->friendId);
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "�������ݿ�ʧ�ܣ�sql:" << sqlBuf << endl;
			return;
		}
		sprintf_s(sqlBuf, "insert into t_friend values ('%d','%d');", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "�������ݿ�ʧ�ܣ�sql:" << sqlBuf << endl;
			return;
		}
		//���º����б�
		getUserList(rs->friendId);

	}

	//3������ͬ�⻹�Ǿܾ���Ӻ��ѣ���Ҫ���ͻ���ת�����ѻظ��Ľṹ��
	m_pMediator->SendData(m_mapIdToSocket[rs->userId], buf, nLen);
}

//������������
void CKernel::dealOfflineRq(long lSendIp, char* buf, int nLen) {//Ҫ������ĳ�ʼ��Э���������Э��ͷ�ʹ����������������������ݲ����ߵ���Ӧ�Ĵ���������
	cout << "CKernel::dealOfflineRq" << endl;
	//1�����
	STRU_TCP_OFFLINE_RQ* rq =(STRU_TCP_OFFLINE_RQ*)buf;

	//��ѯ�����û��ĺ����б�
	list<string> resultList;//��ѯ���
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA='%d';", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//������sql��ѯ��䣬��ѯ��������ѯ���Ľ�����������
		cout << "��ѯ���ݿ�ʧ��,sql:" << sqlBuf << endl;
		return;
	}

	//3����������id�б�
	int friendId=0;
	while (resultList.size()>0) {
		//ȡ������id
		friendId = atoi(resultList.front().c_str());
		resultList.pop_front();

		//4������������ߣ�������ת����������
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
		}
	}

	//5���������û���socket�رգ�����map���Ƴ�
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}