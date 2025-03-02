#include "TcpClientNet.h"
#include "packDef.h"
#include <process.h>
#include "TcpClientMediator.h"

TcpClientNet::TcpClientNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(NULL), m_isStop(false){
	m_pMediator = pMediator;
}
TcpClientNet::~TcpClientNet() {
	UninitNet();
}

//初始化网络:加载库、创建套接字、连接服务端、创建一个接收数据的线程

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
    serverAddr.sin_port = htons(_DEF_TCP_SERVER_PORT);//端口号，84567随便写的，htons--转换成网络字节序,也就是转换成大端存储
    serverAddr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);//添加服务器所在的地址
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
        //_beginthreadex和_endthreadex 一起使用的，_endthreadex会在结束线程的时候先回收空间，再调用ExitThread退出线程
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
    if (m_sock && m_sock != INVALID_SOCKET) {
        closesocket(m_sock);
    }
    //2、卸载库
    WSACleanup();

    //3、优雅的结束线程工作(标志位置成false，等待线程自己结束工作，如果一定时间内，线程不能结束工作，再强制杀死线程
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {
            //在100ms时间内，线程没有退出，就强制杀死
            TerminateThread(m_handle, -1);

        }
        //4、回收句柄
        CloseHandle(m_handle);
        m_handle = NULL;

    }
}
//发送数据
bool TcpClientNet::SendData(long lSendIp, char* buf, int nLen) {
	//1、校验参数
    if (!buf || nLen <= 0) {
        cout << "TcpClientNet::SendData parameter error" << endl;
        return false;
    }

    //2、先发包大小
    if (send(m_sock,(char*) & nLen,sizeof(int),0) <= 0) {
        return false;
    }
    //3、再发包大小
    if (send(m_sock, buf, nLen, 0) <= 0) {
        return false;
    }    
    return true;
}
//接收数据
void TcpClientNet::RecvData() {
    int packSize = 0;
    int nRecvNum = 0;
    int offset = 0;//累计发送数据的大小
    while (!m_isStop) {
        //先接收包大小
        nRecvNum=recv(m_sock, (char*)&packSize, sizeof(int), 0);
        if (nRecvNum > 0) {//如果接收成功
            //new 一个新的空间来接收包内容
            char* packBuf = new char[packSize];
            //再接收包的内容
            while (packSize) {
                nRecvNum = recv(m_sock, packBuf + offset, packSize, 0);
                offset += nRecvNum;
                packSize -= nRecvNum;
            }
                //把接收到的数据传给中介者类
                m_pMediator->DealData(m_sock,packBuf, offset);
                //offset 需要清零，以备下一次使用
                offset = 0;
        }else if(10053==WSAGetLastError()){//手动断开连接的情况（关闭登录注册窗口）
            break;
        }else {
            cout << "TcpClientNet::RecvData recv error:" << WSAGetLastError() << endl;
            break;
        }
        
    }
}
