#pragma once
#define _DEF_UDP_SERVER_PORT (84567)
#define _DEF_NAME_SIZE     (100)
#define _DEF_CONTENT_SIZE  (1024)
#define _DEF_TCP_SERVER_IP  ("192.168.229.1")
#define _DEF_TCP_SERVER_PORT (84563)
#define _DEF_TCP_PROTOCOL_COUNT  (10)//协议头个数，之后功能增加，协议头数增加时，修改该处宏

//请求协议头
#define _DEF_PROTOCOL_BASE  (1000)//1000是任意给的

//上线请求
#define _DEF_PROTOCOL_UDP_ONLINE_RQ   (_DEF_PROTOCOL_BASE+1)
//上线回复
#define _DEF_PROTOCOL_UDP_ONLINE_RS   (_DEF_PROTOCOL_BASE+2)
//下线请求
#define _DEF_PROTOCOL_UDP_OFFLINE_RQ   (_DEF_PROTOCOL_BASE+3)
//聊天请求
#define _DEF_PROTOCOL_UDP_CHAT_RQ   (_DEF_PROTOCOL_BASE+4)
//因为定义的结构体里虽然包含很多传入的信息，但是作为参数传入的时候是强转成char*，这样就分不清传入的数据了，
// 所以我们自定义协议头，通过自定义的计算来区分是那个请求，然后再强转成对应的请求结构体

//请求结构体
//上线请求:协议头、主机名、IP地址
struct STRU_ONLINE {//结构体数据的存储是按定义的顺序存储的
	STRU_ONLINE() :nType(_DEF_PROTOCOL_UDP_ONLINE_RQ) {//初始化
		memset(name, 0, _DEF_NAME_SIZE);
	}
	int nType;
	char name[_DEF_NAME_SIZE];
	//long ip; IP地址是必须要知道，但是没必要在这个结构体里带进去，因为UdpNet中的recvfrom函数中的第五个参数sockAddrClient中已经带有发送端的IP地址信息
};
//上线回复：协议头、主机名---跟上线请求一样，所以用一个结构体


//下线请求：协议头
struct STRU_OFFLINE_RQ {
	STRU_OFFLINE_RQ() :nType(_DEF_PROTOCOL_UDP_OFFLINE_RQ) {}
	int nType;
};

//聊天请求:协议头、聊天内容
struct STRU_CHAT_RQ {
	STRU_CHAT_RQ() :nType(_DEF_PROTOCOL_UDP_CHAT_RQ) {
		memset(content, 0, _DEF_CONTENT_SIZE);
	}
	int nType;
	char content[_DEF_CONTENT_SIZE];
};

//TCP协议头
// 注册请求
#define _DEF_PROTOCOL_TCP_REGISTER_RQ   (_DEF_PROTOCOL_BASE +10)
// 注册回复
#define _DEF_PROTOCOL_TCP_REGISTER_RS   (_DEF_PROTOCOL_BASE + 11)
// 登录请求
#define _DEF_PROTOCOL_TCP_LOGIN_RQ      (_DEF_PROTOCOL_BASE +12)
// 登录回复
#define _DEF_PROTOCOL_TCP_LOGIN_RS      (_DEF_PROTOCOL_BASE+ 13)
// 好友信息
#define _DEF_PROTOCOL_TCP_FRIEND_INFO   (_DEF_PROTOCOL_BASE+ 14)
// 聊天请求
#define _DEF_PROTOCOL_TCP_CHAT_RQ        (_DEF_PROTOCOL_BASE + 15)
// 聊天回复
#define _DEF_PROTOCOL_TCP_CHAT_RS       (_DEF_PROTOCOL_BASE + 16)
// 添加好友请求
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RQ  (_DEF_PROTOCOL_BASE +17)
// 添加好友回复
#define _DEF_PROTOCOL_TCP_ADD_FRIEND_RS (_DEF_PROTOCOL_BASE+ 18)
// 下线
#define _DEF_PROTOCOL_TCP_OFFLINE_RQ     (_DEF_PROTOCOL_BASE + 19)


//请求结果宏定义
//注册结果
#define register_success    (0)  //注册成功
#define name_is_exist       (1)  //昵称已存在、昵称重复
#define tel_is_exist        (2)  //手机号以注册过

// 登录结果
#define login_success (0)
#define password_error (1)
#define user_not_exist (2)

// 用户状态
#define status_online (0)
#define status_offline (1)

// 聊天请求结果
#define send_success (0)
#define send_fail (1)

// 添加好友请求结果
#define add_success (0)
#define no_this_user (1)
#define user_offline (2)
#define user_refuse  (3)




// TCP请求结构体
typedef int PackType;
//注册请求：协议头、昵称、密码、手机号
typedef struct STRU_REGISTER_RQ {
	STRU_REGISTER_RQ() :type(_DEF_PROTOCOL_TCP_REGISTER_RQ) {
		memset(name,0, _DEF_NAME_SIZE);
		memset(password, 0, _DEF_NAME_SIZE);
		memset(tel,0,_DEF_NAME_SIZE);
	}
	PackType type;
	char name[_DEF_NAME_SIZE];
	//这样设为一个char数组，每次都会申请固定长度，会有点浪费空间，怎么可以优化此处：根据昵称长度动态new空间，用一个char*指向该空间即可
	//为什么不用string：string类型本质上是由char数组实现的，重载了赋值等运算符，所以比char数组很多运算上更方便广泛，长度没有限制是因为内部根据长度的变化也会动态的申请不同长度的char空间进行存储，
	//所以string类型的变量长度不是固定的，因此含有string类型的类或结构体的大小也是不固定的
	char password[_DEF_NAME_SIZE];
	char tel[_DEF_NAME_SIZE];
}STRU_REGISTER_RQ;

//注册回复：协议头、注册结果
typedef struct STRU_REGISTER_RS {
	STRU_REGISTER_RS():type(_DEF_PROTOCOL_TCP_REGISTER_RS), result(tel_is_exist) {
	}
	PackType type;
	int result;
}STRU_REGISTER_RS;

//登录请求：协议头、电话号码、密码
typedef struct STRU_LOGIN_RQ {
	STRU_LOGIN_RQ() :type(_DEF_PROTOCOL_TCP_LOGIN_RQ) {
		memset(password,(0),_DEF_NAME_SIZE);
		memset(tel,(0),_DEF_NAME_SIZE);
	}
	PackType type;
	char password[_DEF_NAME_SIZE];
	char tel[_DEF_NAME_SIZE];
}STRU_LOGIN_RQ;

//登录回复：协议头、登录结果，自己的id
typedef struct STRU_LOGIN_RS {
	STRU_LOGIN_RS() : type(_DEF_PROTOCOL_TCP_LOGIN_RS), result(user_not_exist),userId(0) {

}
	PackType type;
	int result;
	int userId;
}STRU_LOGIN_RS;

//好友信息：协议头、用户id、昵称、头像id、在线状态、签名-----界面上显示的内容
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

//聊天请求：协议头、自己id、聊天内容、好友id
typedef struct STRU_TCP_CHAT_RQ {
	STRU_TCP_CHAT_RQ():type(_DEF_PROTOCOL_TCP_CHAT_RQ),userId(0), friendId(0)
	{
		memset(content, 0, _DEF_CONTENT_SIZE); }
	PackType type;
	int userId;
	int friendId;
	char content[_DEF_CONTENT_SIZE];
}STRU_TCP_CHAT_RQ;

//聊天回复：协议头、自己的id、好友的id、请求结果
typedef struct STRU_TCP_CHAT_RS {
STRU_TCP_CHAT_RS() : type (_DEF_PROTOCOL_TCP_CHAT_RS), userId(0), friendId(0), result(send_fail){
}
PackType type;
int userId;
int friendId;
int result;
}STRU_TCP_CHAT_RS;

//添加好友请求：协议头、自己的id、好友的昵称、自己的昵称
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


//添加好友回复：协议头、自己的id、好友的id、添加结果、好友的昵称
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

//下线：协议头、自己的id
typedef struct STRU_TCP_OFFLINE_RQ {
	STRU_TCP_OFFLINE_RQ() :type(_DEF_PROTOCOL_TCP_OFFLINE_RQ), userId(0) {
	}
	PackType type;
	int userId;
}STRU_TCP_OFFLINE_RQ;
