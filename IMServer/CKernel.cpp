#include "CKernel.h"
#include "TcpServerMediator.h"
//#include <list>

//定义计算数组下标的宏替换公式
#define NetProtocolMap(a) m_netProtocolMap[a - _DEF_PROTOCOL_BASE - 10]

//静态成员变量类外初始化
CKernel* CKernel::pKernel = NULL;
CKernel::CKernel() {
	pKernel = this;//这样指针就指向了CKernel类
	setProtocolMap();
}
CKernel::~CKernel() {
	closeServer();
}
//打开服务器:打开网络，连接上数据库
bool CKernel::startServer() {
	//1、打开网络
	m_pMediator = new TcpServerMediator;
	if (!m_pMediator->OpenNet()) {
		cout << "打开网络失败" << endl;
		return false;
	}
	//2、连接上数据库
	if (!sql.ConnectMySql("127.0.0.1","root","","im")) {//127.0.0.1本地回环地址，用户名，密码，库名
		cout << "连接数据库失败" << endl;
		return false;
	}
	return true;
}

//关闭服务器
void CKernel::closeServer() {
	//回收资源
	if (m_pMediator) {
		m_pMediator->CloseNet();
		delete m_pMediator;
		m_pMediator = NULL;
	}

}
//初始化协议数组
void CKernel::setProtocolMap() {
	//1、初始化数组
	memset(m_netProtocolMap, 0, sizeof(pfun)*_DEF_TCP_PROTOCOL_COUNT);

	//2、绑定协议头和处理函数（把处理函数的地址放在对应的下标里）
	NetProtocolMap(_DEF_PROTOCOL_TCP_REGISTER_RQ) = &CKernel::dealRegisterRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_LOGIN_RQ) = &CKernel::dealLoginRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_CHAT_RQ) = &CKernel::dealChatRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ) = &CKernel::dealAddFriendRq;
	NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS) = &CKernel::dealAddFriendRs;
	NetProtocolMap(_DEF_PROTOCOL_TCP_OFFLINE_RQ) = &CKernel::dealOfflineRq;

}

//处理所有收到的数据
void CKernel::dealData(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealData" << endl;
	//1、取出协议头
	int type = *(int*)buf;
	//2、根据协议头判断是哪个结构体，走对应的处理流程
	//2.1、判断协议头是否在有效值范围内
	if ((type - _DEF_PROTOCOL_BASE - 10) >= 0 &&
		(type - _DEF_PROTOCOL_BASE - 10) < (_DEF_TCP_PROTOCOL_COUNT)) {
		//2.2、通过计算数组下标，取出函数地址
		pfun pf = NetProtocolMap(type);
		// 2.3、调用函数
		if (pf) {
			(this->*pf)(lSendIp, buf, nLen);
		}
		else {
			cout << "type:" << type<<endl;
		}
	}
	//3、处理完数据，回收空间
	delete[] buf;

}

//处理注册请求
void CKernel::dealRegisterRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealRegisterRq" << endl;
	//1、拆包：将char*拆为对应结构体
	STRU_REGISTER_RQ* rq = (STRU_REGISTER_RQ*)buf;//（为什么定义为指针STRU_REGISTER_RQ*，因为buf为指针，指针与指针才能强转）
	cout << "tel:" << rq->tel << ",name:" << rq->name << ",password:" << rq->password<<endl;
	
	//2、校验用户输入参数是否合法，校验规则与客户一致
	// （因为收到的请求可能是其他人仿冒的非法请求，所以客户端和服务端都必须校验）---客户端已经写过了，这里没有再写，但是是需要的

	STRU_REGISTER_RS rs;
	//3、根据电话号码查询数据库中是否已经有这个人了
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";//存储SQL语句
	sprintf_s(sqlBuf, "select tel from t_user where tel='%s';",rq->tel);//sprintf_s(),往sqlBuf中写入"..."  sql语句
	if (!sql.SelectMySql(sqlBuf,1,resultList)) {//SelectMySql()查询数据库，参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" <<sqlBuf<< endl;
		return;
	}
	//4、遍历查询结果
	if (resultList.size() > 0) {
		//说明数据库中已经存在这个电话号码注册过的账号了，设置注册回复错误码
		rs.result = tel_is_exist;
	}
	else {
		//没人使用这个电话号码注册
		//5、根据昵称查询数据库中是否已经有这个人
		sprintf_s(sqlBuf, "select name from t_user where name='%s';", rq->name);//往sqlBuf中写入"..."sql语句
		if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
			cout << "查询数据库失败,sql:" << sqlBuf << endl;
			return;
		}

		//6、遍历查询结果
		if (resultList.size() > 0) {
			//说明数据库中已经存在这个昵称注册过的账号了，设置注册回复错误码 
			rs.result = name_is_exist;
		}
		else {
			//手机号和昵称都没有被注册过，可以注册
			//7、把这个人的注册信息写入数据库
			sprintf_s(sqlBuf, "insert into t_user (tel,name,password,icon,feeling) values('%s','%s','%s',0,'啥也没写');",
				rq->tel,rq->name, rq->password);
			if (!sql.UpdateMySql(sqlBuf)) {
				cout << "插入数据库失败, sql:" << sqlBuf << endl;
			}

			//8、设置注册回复结果成功
			rs.result = register_success;
		}

	}
	
	//9、回复客户端注册回复
	m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
}

//处理登录请求
void CKernel::dealLoginRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealLoginRq" << endl;
	//1、拆包：将char*拆为对应结构体
	STRU_LOGIN_RQ* rq = (STRU_LOGIN_RQ*)buf;

	//2、校验用户输入参数是否合法，校验规则与客户一致
	// （因为收到的请求可能是其他人仿冒的非法请求，所以客户端和服务端都必须校验）---客户端已经写过了，这里没有再写，但是是需要的

	//3、根据tel查密码
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id,password from t_user where tel='%s';", rq->tel);//注意这里，先查id在查密码，那么resultList中存储的也是这个顺序，所以在取的时候也应该按照这个顺序取
	if (!sql.SelectMySql(sqlBuf, 2, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" << sqlBuf << endl;
		return;
	}
	STRU_LOGIN_RS rs;
	//4、如果查询结果为空
	if (resultList.size() ==0) {
		//用户不存在，设置登录回复结果为失败
		rs.result = user_not_exist;
		//7、收到登录请求以后，回复给客户端一个登录回复
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
	else {
		//5、如果查询结果不为空，取出用户id和密码(先查id,也先取id)
		int userId = atoi(resultList.front().c_str());//字符串类型id转换成int类型的id
		resultList.pop_front();//弹出id，下一个就是密码

		string pas = resultList.front();
		resultList.pop_front();

		//6、与用户输入的密码进行比较是否相同
		if (0==strcmp(rq->password,pas.c_str())) {//rq->password是char* 所以用char数组的比较方式
			//密码相同，登录成功
			rs.result = login_success;
			rs.userId = userId;//登录成功，记下自己的id
			//保存当前登录成功用户的socket
			m_mapIdToSocket[userId] = lSendIp;
			//7、收到登录请求以后，回复给客户端一个登录回复
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

			//登录成功直接查询当前登录用户的好友信息（包括自己的信息）
			getUserList(userId);
		}
		else {
			//密码不相同，登录失败
			rs.result = password_error;
			//7、收到登录请求以后，回复给客户端一个登录回复
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
		}
	}

	//7、收到登录请求以后，回复给客户端一个登录回复
	//m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));   //不能只到最后在这里发一次登录回复，因为在客户端，是收到登录成功信息后，
	// 有对应的操作（判断是不是自己）,如果只在登录成功也查询好友信息之后回复登录回复，这样会使客户端判断是不是自己的操作没有进行，界面上也不会显示读取到的自己的信息
}

//查询当前登录用户的好友信息（包括自己的信息），传入自己的Id通过自己的id查信息
void CKernel::getUserList(int userId) {
	//1、 查询自己的信息
	STRU_FRIEND_INFO userInfo;
	getUserInfoById(&userInfo, userId);//根据userId查询到该用户的信息，存放在userInfo中

	//2、用userId 取出socket,把自己的信息发给客户端（需要用到socket，有多少个客户端就有多少个socket，所以将socket保存起来,用map,一一对应，便于查找）
	if (m_mapIdToSocket.find(userId) == m_mapIdToSocket.end()) {//没找到
		cout << "CKernel::getUserList获取socket失败，userId="<<userId<<endl;
		return;
	}
	m_pMediator->SendData(m_mapIdToSocket[userId],(char*)&userInfo,sizeof(userInfo));

	//3、先查出有哪些好友
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA='%d';",userId);
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" << sqlBuf << endl;
		return;
	}

	//4、根据好友的id列表，查询出每个好友的信息
	int friendId = 0;
	STRU_FRIEND_INFO friendInfo;
	while (resultList.size() > 0) {
		//5、取出好友id
		friendId = atoi(resultList.front().c_str());
		resultList.pop_front();

		//6、根据好友Id 查询好友信息
		getUserInfoById(&friendInfo, friendId);

		//7、把好友信息发给登录客户端
		m_pMediator->SendData(m_mapIdToSocket[userId], (char*)&friendInfo, sizeof(friendInfo));

		//8、判断好友是否在线   登录成功其socket会存入m_mapIdToSocket，所以通过m_mapIdToSocket判断是否在线
		if (m_mapIdToSocket.find(friendId) != m_mapIdToSocket.end()) {
			//9、好友在线，给好友发送登录人的信息
			m_pMediator->SendData(m_mapIdToSocket[friendId], (char*)&userInfo, sizeof(userInfo));

		}
	}
}

//根据用户id查询用户信息(userInfo是输出参数)
void CKernel::getUserInfoById(STRU_FRIEND_INFO* userInfo, int userId) {
	//1、设置用户id
	userInfo->userId = userId;


	//2、设置用户状态
	if (m_mapIdToSocket.find(userId) != m_mapIdToSocket.end()) {//在线
		userInfo->state = status_online;
	}
	else {
		userInfo->state = status_offline;
	} 

	//3、从数据库查询用户的其他信息
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select name,icon,feeling from t_user where id='%d';", userId);
	if (!sql.SelectMySql(sqlBuf, 3, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" << sqlBuf << endl;
		return;
	}
	//4、获取查询结果，设置到结构体中
	if (resultList.size() == 3) {
		//先取名字
		strcpy_s(userInfo->name, resultList.front().c_str());
		resultList.pop_front();
		//再取icon
		userInfo->iconId=atoi(resultList.front().c_str());
		resultList.pop_front();
		//最后取签名
		strcpy_s(userInfo->feeling, resultList.front().c_str());
		resultList.pop_front();
	}
}

//处理聊天请求（客户端给好友发消息聊天时）
void CKernel::dealChatRq(long lSendIp, char* buf, int nLen) {//lSendIp 发送信息的哪个人的socket
	cout << "CKernel::dealChatRq" << endl;
	//1、拆包
	STRU_TCP_CHAT_RQ* rq = (STRU_TCP_CHAT_RQ*)buf;
	cout << "userId:" << rq->userId << ",friend:" <<rq->friendId<< ",content:" << rq->content<<endl;

	//2、查看好友是否在线
	if (m_mapIdToSocket.count(rq->friendId)>0) {
		//好友在线，就把聊天内容转发给好友
		m_pMediator->SendData(m_mapIdToSocket[rq->friendId], buf, nLen);
	}
	else {
		//好友不在线，回复发送端好友不在线
		STRU_TCP_CHAT_RS rs;
		rs.result = send_fail;
		rs.userId = rq->userId;
		rs.friendId = rq->friendId;
		m_pMediator->SendData(lSendIp,(char*)&rs,sizeof(rs));//lSendIp刚才给我发信息的那个人的socket

	}

}

//处理添加好友请求
void CKernel::dealAddFriendRq(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealAddFriendRq" << endl;

	//1、拆包
	STRU_ADD_FRIEND_RQ* rq = (STRU_ADD_FRIEND_RQ*)buf;

	//2、查看数据库中有没有这个待添加的用户
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select id from t_user where name='%s';", rq->friendName);//这里我们发现我们需要用到好友昵称，所以在客户端处理好友昵称的时候需要转码
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" << sqlBuf << endl;
		return;
	}
	STRU_ADD_FRIEND_RS rs;
	if (resultList.size()>0) {
		//如果有这个待添加用户,取出待添加用户的id
		int friendId = atoi(resultList.front().c_str());
		resultList.pop_front();
		// 3、查看待添加用户是否在线
		if (m_mapIdToSocket.count(friendId) >0) {
			//4、如果待添加用户在线，就把添加好友的请求转发给待添加用户
			m_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
		}
		else {
			//5、如果待添加用户不在线，返回给客户端添加好友失败，原因不在线----TODO:这里可以改进为，不在线时先保存请求信息，等上线时再发送
			rs.result = user_offline;
			rs.friendId = friendId;
			strcpy_s(rs.friendName, rq->friendName);
			rs.userId = rq->userId;
			m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));

		}
	}
	else {
		//6、没有这个待添加用户，返回给客户端添加好友失败，原因用户不存在
		rs.result = no_this_user;
		strcpy_s(rs.friendName, rq->friendName);
		rs.userId = rq->userId;
		m_pMediator->SendData(lSendIp, (char*)&rs, sizeof(rs));
	}
}

//处理添加好友回复
void CKernel::dealAddFriendRs(long lSendIp, char* buf, int nLen) {
	cout << "CKernel::dealAddFriendRs" << endl;
	//1、拆包
	STRU_ADD_FRIEND_RS* rs = (STRU_ADD_FRIEND_RS*)buf;

	//2、如果对方同意添加好友，就把好友关系写入数据库（写入两次，为了保证都成功或者失败，需要使用事务）
	if (add_success == rs->result) {
		char sqlBuf[1024] = "";
		sprintf_s(sqlBuf, "insert into t_friend values ('%d','%d');", rs->userId, rs->friendId);
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据库失败，sql:" << sqlBuf << endl;
			return;
		}
		sprintf_s(sqlBuf, "insert into t_friend values ('%d','%d');", rs->friendId, rs->userId);
		if (!sql.UpdateMySql(sqlBuf)) {
			cout << "插入数据库失败，sql:" << sqlBuf << endl;
			return;
		}
		//更新好友列表
		getUserList(rs->friendId);

	}

	//3、不管同意还是拒绝添加好友，都要给客户端转发好友回复的结构体
	m_pMediator->SendData(m_mapIdToSocket[rs->userId], buf, nLen);
}

//处理下线请求
void CKernel::dealOfflineRq(long lSendIp, char* buf, int nLen) {//要在上面的初始化协议数组里绑定协议头和处理函数，这样发过来的数据才能走到对应的处理函数里面
	cout << "CKernel::dealOfflineRq" << endl;
	//1、拆包
	STRU_TCP_OFFLINE_RQ* rq =(STRU_TCP_OFFLINE_RQ*)buf;

	//查询下线用户的好友列表
	list<string> resultList;//查询结果
	char sqlBuf[1024] = "";
	sprintf_s(sqlBuf, "select idB from t_friend where idA='%d';", rq->userId);
	if (!sql.SelectMySql(sqlBuf, 1, resultList)) {//参数：sql查询语句，查询列数，查询到的结果放入的容器
		cout << "查询数据库失败,sql:" << sqlBuf << endl;
		return;
	}

	//3、遍历好友id列表
	int friendId=0;
	while (resultList.size()>0) {
		//取出好友id
		friendId = atoi(resultList.front().c_str());
		resultList.pop_front();

		//4、如果好友在线，给好友转发下线请求
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pMediator->SendData(m_mapIdToSocket[friendId], buf, nLen);
		}
	}

	//5、把下线用户的socket关闭，并从map中移除
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		closesocket(m_mapIdToSocket[rq->userId]);
		m_mapIdToSocket.erase(rq->userId);
	}
}