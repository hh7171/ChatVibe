#include "TcpClientNet.h"
#include <process.h>
#include "TcpClientMediator.h"
#include "packDef.h"

TcpClientNet::TcpClientNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(NULL), m_isStop(false){//bool值要初始化，不初始化默认true
	m_pMediator = pMediator;
}
TcpClientNet::~TcpClientNet() {
	UninitNet();
}

//初始化网络:加载库、创建套接字、连接服务端、创建一个接收数据的线程（客户端）
bool TcpClientNet::InitNet() {
    //1、选项目--加载库WSAStartup()
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        printf("WSAStartup failed with error: %d\n", err);
        return false;
    }


    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        printf("Could not find a usable version of Winsock.dll\n");
        return false;
    }
    else {
        printf("The Winsock 2.2 dll was found okay\n");
    }

    //2、雇帮手--创建套接字socket()
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//更改为TCP的套接字（客户端只有一个socket，所以用sock表示即可）
    //m_sock 成员变量，不必设为全局变量，只在几个成员函数（下面的收发数据，关闭网络函数）中使用，记得在构造函数里面初始化
    if (INVALID_SOCKET == m_sock) {//如果返回值不合法
        cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()获取错误码，通过->工具-错误查找,可以查找错误在哪里
        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3、连接服务端
    sockaddr_in serverAddr;//定义一个sockaddr_in类型变量，存储socket地址信息
    serverAddr.sin_family = AF_INET;//地址组
    serverAddr.sin_port = htons(_DEF_TCP_SERVER_PORT);//端口号，84567、84563随便写的，htons--转换成网络字节序,也就是转换成大端存储
    serverAddr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);    //添加服务器所在的地址
    err = connect(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (SOCKET_ERROR == err) {//如果返回值不合法
        cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()获取错误码，通过->工具-错误查找,可以查找错误在哪里
        return false;
    }
    else {
        cout << "connect success" << endl;//打印出connect success这个，只能证明调用connect这个函数成功了，不能证明，连接成功了
        //只要服务端打印出客户端的IP地址，才能说明连接成功
    }

    //4、创建一个接收数据的线程
       //CreateThread 和 ExitThread是一起使用的，如果在线程中使用了C++运行时库的函数，例如strcpy会申请一个空间，但是使用ExitThread退出线程的时候，
        //不会回收申请的空间，就会导致空间泄漏
        //_beginthreadex和_endthreadex 一起使用的，_endthreadex会在结束线程的时候先回收空间，再调用ExitThread退出线程，头文件<process.h>
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    return true;
}

    //接收数据的线程函数
    unsigned _stdcall TcpClientNet::RecvThread(void* lpVoid) {
        TcpClientNet* pThis = (TcpClientNet*)lpVoid;
        pThis->RecvData();
        return 0;
    }

//关闭网络:关闭套接字、卸载库、结束线程+回收句柄（为了回收内核对象）
void TcpClientNet::UninitNet() {
    //1、关闭套接字
    if (m_sock && m_sock != INVALID_SOCKET) {//存在且有效（INVALID_SOCKET无效）
        closesocket(m_sock);
    }
    //2、卸载库
    WSACleanup();

    //3、优雅的结束线程工作(标志位置成false，等待线程自己结束工作，如果一定时间内，线程不能结束工作，再强制杀死线程
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {//WAIT_TIMEOUT超时
            //在100ms时间内，线程没有退出，就强制杀死
            TerminateThread(m_handle, -1);

        }
        //4、回收句柄
        CloseHandle(m_handle);
        m_handle = NULL;

    }
}

//发送数据
//TCP是基于字节流的传输，存在粘包问题
bool TcpClientNet::SendData(long lSendIp, char* buf, int nLen) {//客户端的SendData函数参数lSendIp没有使用，客户端只与服务端通信，而服务端与服务端通信的socket已经存成成员变量了，
    //所以在调用该函数时，第一个参数随便传一个就行，这就是为什么有时候调用该函数时，第一个函数参数传0
	//1、校验参数
    if (!buf || nLen <= 0) {
        cout << "TcpClientNet::SendData parameter error" << endl;
        return false;
    }
    //2、先发包大小
    if (send(m_sock,(char*) & nLen,sizeof(int),0) <= 0) {
        return false;
    }
    //3、再发包内容
    if (send(m_sock, buf, nLen, 0) <= 0) {
        return false;
    }    
    return true;
}

//接收数据
void TcpClientNet::RecvData() {
    int packSize = 0;//将接收到的包大小存到packSize中
    int nRecvNum = 0;//
    int offset = 0;//累计发送数据的大小
    while (!m_isStop) {
        //先接收包大小
        nRecvNum=recv(m_sock, (char*)&packSize, sizeof(int), 0);
        if (nRecvNum > 0) {//如果接收成功
            //new 一个新的空间来接收包内容（因为是多线程，防止还没有处理完接收到的数据就有新的数据发送过来将其覆盖，所以每次都要new一个新的空间来存储要接收的数据）
            char* packBuf = new char[packSize];
            //再接收包的内容
            while (packSize) {//写成循环是因为存在数据太多分几个包发送的情况
                nRecvNum = recv(m_sock, packBuf + offset, packSize, 0);//nRecvNum此次接收到的数据大小   packBuf + offset开始写数据的位置   packSize要接收的剩余数据的大小
                //第一个参数是传IP的，但是在TCP中，因为在收发之前已经建立连接了，传输数据只通过之间的sock传输，而sock是SOCKET类型，本质是unsigned long long类型
                offset += nRecvNum;
                packSize -= nRecvNum;
            }
                //把接收到的数据传给中介者类
                m_pMediator->DealData(m_sock,packBuf, offset);//while之后，offset就等于包的大小，packSize为0
                //offset 需要清零，以备下一次使用
                offset = 0;
        }
        else {
            cout << "TcpClientNet::RecvData recv error:" << WSAGetLastError() << endl;
            break;
        }
        
    }
}
