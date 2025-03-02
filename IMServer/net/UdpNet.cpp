#include"UdpNet.h"
#include"UdpMediator.h"
#include "packDef.h"
#include <process.h>

UdpNet::UdpNet(INetMediator* pMediator):m_handle(0),m_sock(INVALID_SOCKET),m_isStop(false) {//���캯����ʼ��
	//m_pMediator = new UdpMediator; �������г�ͻ����UdpNet��newһ��UdpMediator��new UdpMediatorʱ��UdpMediatorҪnewһ��UdpNet
	//�����������UdpNet�Ĺ��캯����Ϊһ���вι���
	m_pMediator = pMediator;
}
UdpNet::~UdpNet() {
	UninitNet();
}
//��ʼ�����磺���ؿ⡢�����׽��֡���IP��ַ������㲥Ȩ�ޣ����޹㲥��ַ���������������ݵ��̣߳����߳���һֱ�ȴ��������ݣ����Ҳ�Ӱ�����������У�
bool UdpNet::InitNet() {

        //1��ѡ��Ŀ--���ؿ�WSAStartup()
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

        /* Confirm that the WinSock DLL supports 2.2.*/
        /* Note that if the DLL supports versions greater    */
        /* than 2.2 in addition to 2.2, it will still return */
        /* 2.2 in wVersion since that is the version we      */
        /* requested.                                        */

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            printf("Could not find a usable version of Winsock.dll\n");
            return false;
        }
        else {
            printf("The Winsock 2.2 dll was found okay\n");
        }

        //2���Ͱ���--�����׽���socket()
        m_sock= socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (INVALID_SOCKET == m_sock) {//�������ֵ���Ϸ�
            cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
            return false;
        }
        else {
            cout << "socket success" << endl;
        }

        //3��ѡ��ַ--��IP��ַbind()
        sockaddr_in serverAddr;//sockaddr_inʵ���ǵȼ۵ģ�����sockaddr_in���壬Ȼ��ǿתΪsockaddr
        serverAddr.sin_family = AF_INET;//��ַ��
        serverAddr.sin_port = htons(_DEF_UDP_SERVER_PORT);//�˿ںţ�84567���д�ģ�htons--ת���������ֽ���,Ҳ����ת���ɴ�˴洢
        /*
        *serverAddr.sin_addr.S_un.S_addr =inrt_addr("192.168.213.123");
        * ʮ����4�ȷ��ַ������͵�ip��ַ��ʹ��inrt_addr()
        *�ͻ��˴������
        */
        serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;//�������Ҫ����������    
        err = bind(m_sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));//������socket,ָ��sockaddr�ṹ���ָ�루���涨��һ��sockaddr�ṹ�壩��sockaddr�ṹ��ĳ���,ע��&serverAddr��ȡ��ַ
        if (SOCKET_ERROR == err) {//�������ֵ���Ϸ�
            cout << "bind error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
            return false;
        }
        else {
            cout << "bind success" << endl;
        }
        //ʹ�����޹㲥��ַ����Ҫ������㲥Ȩ��
        //4������Ȩ�ޣ�
        bool bral = true;
        setsockopt(m_sock, SOL_SOCKET, SO_BROADCAST, (char*)&bral, sizeof(bral));

       //5�� �����������ݵ��߳�
       //CreateThread �� ExitThread��һ��ʹ�õģ�������߳���ʹ����C++����ʱ��ĺ���������strcpy������һ���ռ䣬����ʹ��ExitThread�˳��̵߳�ʱ��
        //�����������Ŀռ䣬�ͻᵼ�¿ռ�й©
        //_beginthreadex��_endthreadex һ��ʹ�õģ�_endthreadex���ڽ����̵߳�ʱ���Ȼ��տռ䣬�ٵ���ExitThread�˳��߳�
        m_handle =(HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
        return true;
}
//�������ݵ��̺߳���
unsigned _stdcall UdpNet::RecvThread(void* lpVoid) {
    UdpNet* pThis = (UdpNet*)lpVoid;
    pThis->RecvData();
    return 0;
}

//�ر����磺�����̹߳�����ͨ����Ա������־λ�����رվ���������ں˶��󣩡��ر��׽��֡�ж�ؿ�
void UdpNet::UninitNet() {
    //1. �����̹߳���
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 500)) {
            TerminateThread(m_handle, -1);
        }
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    
    //2. ���վ��
    if (m_handle) {
        CloseHandle(m_handle);
        m_handle = NULL;
    }
    //3.�ر��׽���
    if (!m_sock&& INVALID_SOCKET!=m_sock) {
        closesocket(m_sock);
    }

    //4.ж�ؿ�
    WSACleanup();
}
//��������
bool UdpNet::SendData(long lSendIp, char* buf, int nLen) {
    sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(_DEF_UDP_SERVER_PORT);
    sockAddr.sin_addr.S_un.S_addr = lSendIp;
    if (sendto(m_sock,buf,nLen,0, (sockaddr*) & sockAddr,sizeof(sockAddr)) <= 0) {
        cout << "UdpNet::SendData error:" << WSAGetLastError() << endl;
       return false;
    }
	return true;
}
//��������
void UdpNet::RecvData() {
    int nRecvNum = 0;
    char recvBuf[4096] = "";
    sockaddr_in sockAddrClient;
    int sockAddrClientSize = sizeof(sockAddrClient);
   while (!m_isStop) {
       nRecvNum=recvfrom(m_sock,recvBuf,sizeof(recvBuf),0,(sockaddr*)&sockAddrClient,&sockAddrClientSize);
       if (nRecvNum > 0) {
           //newһ���µĿռ䣬�������յ����ݵ��µĿռ䣬���µĿռ䴫�ݸ��н�����
           char* packBuf = new char[nRecvNum];
           //�������ݣ�����strcpy�����ַ���������\0Ϊ��β����������������û�п���
           memcpy(packBuf, recvBuf, nRecvNum);
           //�����յ������ݴ��ݸ��н���
           m_pMediator->DealData(sockAddrClient.sin_addr.S_un.S_addr, packBuf, nRecvNum);
       }
   }
}