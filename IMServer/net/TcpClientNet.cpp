#include "TcpClientNet.h"
#include <process.h>
#include "TcpClientMediator.h"
#include "packDef.h"

TcpClientNet::TcpClientNet(INetMediator* pMediator):m_sock(INVALID_SOCKET),m_handle(NULL), m_isStop(false){//boolֵҪ��ʼ��������ʼ��Ĭ��true
	m_pMediator = pMediator;
}
TcpClientNet::~TcpClientNet() {
	UninitNet();
}

//��ʼ������:���ؿ⡢�����׽��֡����ӷ���ˡ�����һ���������ݵ��̣߳��ͻ��ˣ�
bool TcpClientNet::InitNet() {
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
    m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����ΪTCP���׽��֣��ͻ���ֻ��һ��socket��������sock��ʾ���ɣ�
    //m_sock ��Ա������������Ϊȫ�ֱ�����ֻ�ڼ�����Ա������������շ����ݣ��ر����纯������ʹ�ã��ǵ��ڹ��캯�������ʼ��
    if (INVALID_SOCKET == m_sock) {//�������ֵ���Ϸ�
        cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3�����ӷ����
    sockaddr_in serverAddr;//����һ��sockaddr_in���ͱ������洢socket��ַ��Ϣ
    serverAddr.sin_family = AF_INET;//��ַ��
    serverAddr.sin_port = htons(_DEF_TCP_SERVER_PORT);//�˿ںţ�84567��84563���д�ģ�htons--ת���������ֽ���,Ҳ����ת���ɴ�˴洢
    serverAddr.sin_addr.S_un.S_addr = inet_addr(_DEF_TCP_SERVER_IP);    //��ӷ��������ڵĵ�ַ
    err = connect(m_sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (SOCKET_ERROR == err) {//�������ֵ���Ϸ�
        cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
        return false;
    }
    else {
        cout << "connect success" << endl;//��ӡ��connect success�����ֻ��֤������connect��������ɹ��ˣ�����֤�������ӳɹ���
        //ֻҪ����˴�ӡ���ͻ��˵�IP��ַ������˵�����ӳɹ�
    }

    //4������һ���������ݵ��߳�
       //CreateThread �� ExitThread��һ��ʹ�õģ�������߳���ʹ����C++����ʱ��ĺ���������strcpy������һ���ռ䣬����ʹ��ExitThread�˳��̵߳�ʱ��
        //�����������Ŀռ䣬�ͻᵼ�¿ռ�й©
        //_beginthreadex��_endthreadex һ��ʹ�õģ�_endthreadex���ڽ����̵߳�ʱ���Ȼ��տռ䣬�ٵ���ExitThread�˳��̣߳�ͷ�ļ�<process.h>
    m_handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, this, 0, NULL);
    return true;
}

    //�������ݵ��̺߳���
    unsigned _stdcall TcpClientNet::RecvThread(void* lpVoid) {
        TcpClientNet* pThis = (TcpClientNet*)lpVoid;
        pThis->RecvData();
        return 0;
    }

//�ر�����:�ر��׽��֡�ж�ؿ⡢�����߳�+���վ����Ϊ�˻����ں˶���
void TcpClientNet::UninitNet() {
    //1���ر��׽���
    if (m_sock && m_sock != INVALID_SOCKET) {//��������Ч��INVALID_SOCKET��Ч��
        closesocket(m_sock);
    }
    //2��ж�ؿ�
    WSACleanup();

    //3�����ŵĽ����̹߳���(��־λ�ó�false���ȴ��߳��Լ��������������һ��ʱ���ڣ��̲߳��ܽ�����������ǿ��ɱ���߳�
    m_isStop = true;
    if (m_handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 100)) {//WAIT_TIMEOUT��ʱ
            //��100msʱ���ڣ��߳�û���˳�����ǿ��ɱ��
            TerminateThread(m_handle, -1);

        }
        //4�����վ��
        CloseHandle(m_handle);
        m_handle = NULL;

    }
}

//��������
//TCP�ǻ����ֽ����Ĵ��䣬����ճ������
bool TcpClientNet::SendData(long lSendIp, char* buf, int nLen) {//�ͻ��˵�SendData��������lSendIpû��ʹ�ã��ͻ���ֻ������ͨ�ţ��������������ͨ�ŵ�socket�Ѿ���ɳ�Ա�����ˣ�
    //�����ڵ��øú���ʱ����һ��������㴫һ�����У������Ϊʲô��ʱ����øú���ʱ����һ������������0
	//1��У�����
    if (!buf || nLen <= 0) {
        cout << "TcpClientNet::SendData parameter error" << endl;
        return false;
    }
    //2���ȷ�����С
    if (send(m_sock,(char*) & nLen,sizeof(int),0) <= 0) {
        return false;
    }
    //3���ٷ�������
    if (send(m_sock, buf, nLen, 0) <= 0) {
        return false;
    }    
    return true;
}

//��������
void TcpClientNet::RecvData() {
    int packSize = 0;//�����յ��İ���С�浽packSize��
    int nRecvNum = 0;//
    int offset = 0;//�ۼƷ������ݵĴ�С
    while (!m_isStop) {
        //�Ƚ��հ���С
        nRecvNum=recv(m_sock, (char*)&packSize, sizeof(int), 0);
        if (nRecvNum > 0) {//������ճɹ�
            //new һ���µĿռ������հ����ݣ���Ϊ�Ƕ��̣߳���ֹ��û�д�������յ������ݾ����µ����ݷ��͹������串�ǣ�����ÿ�ζ�Ҫnewһ���µĿռ����洢Ҫ���յ����ݣ�
            char* packBuf = new char[packSize];
            //�ٽ��հ�������
            while (packSize) {//д��ѭ������Ϊ��������̫��ּ��������͵����
                nRecvNum = recv(m_sock, packBuf + offset, packSize, 0);//nRecvNum�˴ν��յ������ݴ�С   packBuf + offset��ʼд���ݵ�λ��   packSizeҪ���յ�ʣ�����ݵĴ�С
                //��һ�������Ǵ�IP�ģ�������TCP�У���Ϊ���շ�֮ǰ�Ѿ����������ˣ���������ֻͨ��֮���sock���䣬��sock��SOCKET���ͣ�������unsigned long long����
                offset += nRecvNum;
                packSize -= nRecvNum;
            }
                //�ѽ��յ������ݴ����н�����
                m_pMediator->DealData(m_sock,packBuf, offset);//while֮��offset�͵��ڰ��Ĵ�С��packSizeΪ0
                //offset ��Ҫ���㣬�Ա���һ��ʹ��
                offset = 0;
        }
        else {
            cout << "TcpClientNet::RecvData recv error:" << WSAGetLastError() << endl;
            break;
        }
        
    }
}
