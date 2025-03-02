#pragma once
#define _DEF_UDP_SERVER_PORT (84567)
#define _DEF_NAME_SIZE     (100)
#define _DEF_CONTENT_SIZE  (1024)
#define _DEF_TCP_SERVER_IP  ("10.28.5.8")
#define _DEF_TCP_SERVER_PORT (84563)
#define _DEF_TCP_PROTOCOL_COUNT  (10)//Э��ͷ������֮�������ӣ�Э��ͷ������ʱ���޸ĸô���


//����Э��ͷ
#define _DEF_PROTOCOL_BASE  (1000)//1000���������
//��UDP������ʱ�õģ�
//��������
#define _DEF_PROTOCOL_UDP_ONLINE_RQ   (_DEF_PROTOCOL_BASE+1)
//���߻ظ�
#define _DEF_PROTOCOL_UDP_ONLINE_RS   (_DEF_PROTOCOL_BASE+2)
//��������
#define _DEF_PROTOCOL_UDP_OFFLINE_RQ   (_DEF_PROTOCOL_BASE+3)
//��������
#define _DEF_PROTOCOL_UDP_CHAT_RQ   (_DEF_PROTOCOL_BASE+4)
//��Ϊ����Ľṹ������Ȼ�����ܶഫ�����Ϣ��������Ϊ���������ʱ����ǿת��char*�������ͷֲ��崫��������ˣ�
// ���������Զ���Э��ͷ��ͨ���Զ���ļ������������Ǹ�����Ȼ����ǿת�ɶ�Ӧ������ṹ��

//����ṹ��
//��������:Э��ͷ����������IP��ַ
struct STRU_ONLINE {//�ṹ�����ݵĴ洢�ǰ������˳��洢��
	STRU_ONLINE() :nType(_DEF_PROTOCOL_UDP_ONLINE_RQ) {//��ʼ��
		memset(name, 0, _DEF_NAME_SIZE);
	}
	int nType;
	char name[_DEF_NAME_SIZE];
	//long ip; IP��ַ�Ǳ���Ҫ֪��������û��Ҫ������ṹ�������ȥ����ΪUdpNet�е�recvfrom�����еĵ��������sockAddrClient���Ѿ����з��Ͷ˵�IP��ַ��Ϣ
};
//���߻ظ���Э��ͷ��������---����������һ����������һ���ṹ��


//��������Э��ͷ
struct STRU_OFFLINE_RQ {
	STRU_OFFLINE_RQ() :nType(_DEF_PROTOCOL_UDP_OFFLINE_RQ) {}
	int nType;
};

//��������:Э��ͷ����������
struct STRU_CHAT_RQ {
	STRU_CHAT_RQ() :nType(_DEF_PROTOCOL_UDP_CHAT_RQ) {
		memset(content, 0, _DEF_CONTENT_SIZE);
	}
	int nType;
	char content[_DEF_CONTENT_SIZE];
};

//TCPЭ��ͷ
// ע������
#define _DEF_PROTOCOL_TCP_REGISTER_RQ    (_DEF_PROTOCOL_BASE +10)
// ע��ظ�
#define _DEF_PROTOCOL_TCP_REGISTER_RS    (_DEF_PROTOCOL_BASE + 11)
// ��¼����
#define _DEF_PROTOCOL_TCP_LOGIN_RQ       (_DEF_PROTOCOL_BASE +12)
// ��¼�ظ�
#define _DEF_PROTOCOL_TCP_LOGIN_RS       (_DEF_PROTOCOL_BASE+ 13)
// ������Ϣ
#define _DEF_PROTOCOL_TCP_FRIEND_INFO    (_DEF_PROTOCOL_BASE+ 14)
// ��������
#define _DEF_PROTOCOL_TCP_CHAT_RQ        (_DEF_PROTOCOL_BASE + 15)
// ����ظ�
#define _DEF_PROTOCOL_TCP_CHAT_RS        (_DEF_PROTOCOL_BASE + 16)
// ��Ӻ�������
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RQ  (_DEF_PROTOCOL_BASE +17)
// ��Ӻ��ѻظ�
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RS  (_DEF_PROTOCOL_BASE+ 18)
// ����
#define _DEF_PROTOCOL_TCP_OFFLINE_RQ     (_DEF_PROTOCOL_BASE + 19)


//�������궨��
//ע����
#define register_success    (0)  //ע��ɹ�
#define name_is_exist       (1)  //�ǳ��Ѵ��ڡ��ǳ��ظ�
#define tel_is_exist        (2)  //�ֻ�����ע���
//�ǳơ����벻�淶�ȼ�У�����⣬���ڿͻ���У�飬����ע����������ˣ������У����ٰѽ�����ظ��ͻ��ˣ��ͻ����ٸ�

// ��¼���
#define login_success (0)
#define password_error (1)
#define user_not_exist (2)

// �û�״̬
#define status_online (0)
#define status_offline (1)

// ����������
#define send_success (0)
#define send_fail (1)

// ��Ӻ���������
#define add_success (0)
#define no_this_user (1)
#define user_offline (2)
#define user_refuse  (3)




// TCP����ṹ��
typedef int PackType;
//ע������Э��ͷ���ǳơ����롢�ֻ���
typedef struct STRU_REGISTER_RQ {
	STRU_REGISTER_RQ() :type(_DEF_PROTOCOL_TCP_REGISTER_RQ) {//��ʼ��
		memset(name,0, _DEF_NAME_SIZE);
		memset(password, 0, _DEF_NAME_SIZE);
		memset(tel,0,_DEF_NAME_SIZE);
	}
	PackType type;
	char name[_DEF_NAME_SIZE];
	//������Ϊһ��char���飬ÿ�ζ�������̶����ȣ����е��˷ѿռ䣬��ô�����Ż��˴��������ǳƳ��ȶ�̬new�ռ䣬��һ��char*ָ��ÿռ伴��
	//Ϊʲô����string��string���ͱ���������char����ʵ�ֵģ������˸�ֵ������������Ա�char����ܶ������ϸ�����㷺������û����������Ϊ�ڲ����ݳ��ȵı仯Ҳ�ᶯ̬�����벻ͬ���ȵ�char�ռ���д洢��
	//����string���͵ı������Ȳ��ǹ̶��ģ���˺���string���͵����ṹ��Ĵ�СҲ�ǲ��̶���
	char password[_DEF_NAME_SIZE];
	char tel[_DEF_NAME_SIZE];
}STRU_REGISTER_RQ;

//ע��ظ���Э��ͷ��ע����
typedef struct STRU_REGISTER_RS {
	STRU_REGISTER_RS():type(_DEF_PROTOCOL_TCP_REGISTER_RS), result(tel_is_exist) {
	}
	PackType type;
	int result;
}STRU_REGISTER_RS;

//��¼����Э��ͷ���绰���롢����
typedef struct STRU_LOGIN_RQ {
	STRU_LOGIN_RQ() :type(_DEF_PROTOCOL_TCP_LOGIN_RQ) {
		memset(password,(0),_DEF_NAME_SIZE);
		memset(tel,(0),_DEF_NAME_SIZE);
	}
	PackType type;
	char password[_DEF_NAME_SIZE];
	char tel[_DEF_NAME_SIZE];
}STRU_LOGIN_RQ;

//��¼�ظ���Э��ͷ����¼������Լ���id
typedef struct STRU_LOGIN_RS {
	STRU_LOGIN_RS() : type(_DEF_PROTOCOL_TCP_LOGIN_RS), result(user_not_exist),userId(0) {

}
	PackType type;
	int result;
	int userId;
}STRU_LOGIN_RS;

//������Ϣ��Э��ͷ���û�id���ǳơ�ͷ��id������״̬��ǩ��-----��������ʾ������
typedef struct STRU_FRIEND_INFO {
	STRU_FRIEND_INFO() : type(_DEF_PROTOCOL_TCP_FRIEND_INFO), userId(0), iconId(0), state(0) {
		memset(name, 0, _DEF_NAME_SIZE);
		memset(feeling, 0,_DEF_NAME_SIZE);
	}
	PackType type;
	int userId;
	int iconId;
	int state;
	char name[_DEF_NAME_SIZE];
	char feeling[_DEF_NAME_SIZE];
}STRU_FRIEND_INFO;

//��������Э��ͷ���Լ�id���������ݡ�����id
typedef struct STRU_TCP_CHAT_RQ {
	STRU_TCP_CHAT_RQ():type(_DEF_PROTOCOL_TCP_CHAT_RQ),userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_SIZE); }
	PackType type;
	int userId;
	int friendId;
	char content[_DEF_CONTENT_SIZE];
}STRU_TCP_CHAT_RQ;

//����ظ���Э��ͷ���Լ���id�����ѵ�id��������
typedef struct STRU_TCP_CHAT_RS {
STRU_TCP_CHAT_RS() : type (_DEF_PROTOCOL_TCP_CHAT_RS), userId(0), friendId(0), result(send_fail){
}
PackType type;
int userId;
int friendId;
int result;
}STRU_TCP_CHAT_RS;

//��Ӻ�������Э��ͷ���Լ���id�����ѵ��ǳơ��Լ����ǳ�
typedef struct STRU_ADD_FRIEND_RQ {
	STRU_ADD_FRIEND_RQ() :type(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ), userId(0) {
		memset(userName, 0, _DEF_NAME_SIZE);
		memset(friendName, 0, _DEF_NAME_SIZE);
	}
	PackType type;
	int userId;
	char userName[_DEF_NAME_SIZE];
	char friendName[_DEF_NAME_SIZE];
}STRU_ADD_FRIEND_RQ;


//��Ӻ��ѻظ���Э��ͷ���Լ���id�����ѵ�id����ӽ�������ѵ��ǳ�
typedef struct STRU_ADD_FRIEND_RS {
	STRU_ADD_FRIEND_RS() :type(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS), userId(0), friendId(0), result(user_refuse) {
		memset(friendName, 0, _DEF_NAME_SIZE);
	}
	PackType type;
	int userId;
	int friendId;
	int result;
	char friendName[_DEF_NAME_SIZE];
}STRU_ADD_FRIEND_RS;

//���ߣ�Э��ͷ���Լ���id
typedef struct STRU_TCP_OFFLINE_RQ {
	STRU_TCP_OFFLINE_RQ() :type(_DEF_PROTOCOL_TCP_OFFLINE_RQ), userId(0) {
	}
	PackType type;
	int userId;
}STRU_TCP_OFFLINE_RQ;