#include "TcpServerNet.h"
#include "packDef.h"
#include <process.h>
#include "TcpServerMediator.h"
TcpServerNet::TcpServerNet(INetMediator* pMediator) :m_sockListen(INVALID_SOCKET),m_isStop(false) {
	m_pMediator = pMediator;
}
TcpServerNet::~TcpServerNet() {
	UninitNet();
}

//��ʼ�����磺���ؿ⡢�����׽��֡���IP��ַ������������һ���������ӵ��̣߳�һֱ�������ӣ�����ֻΪһ���ͻ��˷��񣬶��ҽ������ӻ���һ��������������������Ϊ�������ӵ�������һ���̣߳�
bool TcpServerNet::InitNet() {

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
    //SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����ΪTCP���׽��֣�socket��ֻ���������ʹ�ã����Զ���Ϊ���Ա
    if (INVALID_SOCKET == m_sockListen) {//�������ֵ���Ϸ�
        cout << "socket error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
        return false;
    }
    else {
        cout << "socket success" << endl;
    }

    //3��ѡ��ַ--��IP��ַbind()
    //����˰�IP��ַ��Ŀ����Ϊ����������Ϊ������ṩһ����ȷ�ġ���ʶ���λ��,�Ա�ͻ����ܹ��ҵ�����֮�������ӡ�������̲����漰�󶨿ͻ��˵�ַ,��Ϊ�ͻ��˵�ַ���ɿͻ����ڷ�������ʱ��ָ̬���ġ�
    //һ̨����������ӵ�ж�������Ͷ��IP��ַ��ͨ�����ض���IP��ַ,����˿���ָ��ʹ���ĸ��������ṩ����,�Ӷ����㲻ͬ���������ú���������
    sockaddr_in serverAddr;//sockaddr_inʵ���ǵȼ۵ģ�����sockaddr_in���壬Ȼ��ǿתΪsockaddr
    serverAddr.sin_family = AF_INET;//��ַ��
    serverAddr.sin_port = htons(_DEF_TCP_SERVER_PORT);//�˿ںţ�84567���д�ģ�htons--ת���������ֽ���,Ҳ����ת���ɴ�˴洢
    /*
    *serverAddr.sin_addr.S_un.S_addr =inrt_addr("192.168.213.123");
    * ʮ����4�ȷ��ַ������͵�ip��ַ��ʹ��inrt_addr()
    *�ͻ��˴������
    */
    serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;//�������Ҫ����������    
    err = bind(m_sockListen, (sockaddr*)&serverAddr, sizeof(serverAddr));//������socket,ָ��sockaddr�ṹ���ָ�루���涨��һ��sockaddr�ṹ�壩��sockaddr�ṹ��ĳ���,ע��&serverAddr��ȡ��ַ����ǿת
    if (SOCKET_ERROR == err) {//�������ֵ���Ϸ�
        cout << "bind error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
        return false;
    }
    else {
        cout << "bind success" << endl;
    }

    //4������ ����listen();
    err = listen(m_sockListen, 10);//�ڶ���������ͨ���������ǣ�������ܹ����ӵĿͻ��˶��е���󳤶ȣ����������������ͻ����Ŷӣ�---10�����д��
    if (SOCKET_ERROR == err) {
        cout << "listen error:" << WSAGetLastError() << endl;// WSAGetLastError()��ȡ�����룬ͨ��->����-�������,���Բ��Ҵ���������
        return false;
    }
    else {
        cout << "listen success" << endl;
    }
	
	// 5������һ���������ӵ��߳�
        //CreateThread �� ExitThread��һ��ʹ�õģ�������߳���ʹ����C++����ʱ��ĺ���������strcpy������һ���ռ䣬����ʹ��ExitThread�˳��̵߳�ʱ��
        //�����������Ŀռ䣬�ͻᵼ�¿ռ�й©
        //_beginthreadex��_endthreadex һ��ʹ�õģ�_endthreadex���ڽ����̵߳�ʱ���Ȼ��տռ䣬�ٵ���ExitThread�˳��߳�
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &AcceptThread, this, 0, NULL);
    if (handle) {
        m_listThreadHandle.push_back(handle);
    }
    return true;
	}
//�������ӵ��̺߳���:ѭ���ȴ����ܿͻ�������
unsigned _stdcall TcpServerNet::AcceptThread(void* lpVoid) {
    TcpServerNet* pThis = (TcpServerNet*)lpVoid;
    sockaddr_in addrClient;
    int addrClientSize = sizeof(addrClient);
    while (!pThis->m_isStop) {//m_isStop��̬��Ա��ʹ��this�������
        //��������
       SOCKET sockClient= accept(pThis->m_sockListen, (sockaddr*)&addrClient,&addrClientSize);
       //��ӡ���ӳɹ��Ŀͻ��˵�IP��ַ��ֻҪ��ӡ��ip��ַ����˵���ͻ��˺ͷ�������ӳɹ��ˣ�
       cout << "client ip:" << inet_ntoa(addrClient.sin_addr) <<"connect success"<< endl;//inet_ntoa��ulong����ת���ɳ�����ʮ�����ĵȷֵ�IP��ַ
        //����һ�����ӳɹ��Ŀͻ��˶�Ӧ�Ľ������ݵ��߳�
       unsigned int threadId = 0;
       HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &RecvThread, pThis, 0, &threadId);

       //������ͻ��˶�Ӧ��socket�ŵ�map�й���
       pThis->m_mapThredIdToSocket[threadId] = sockClient;
       //���̵߳ľ������list�������
       if (handle) {
           pThis->m_listThreadHandle.push_back(handle);
       }
    }
    return 0;
}

//�������ݵ��̺߳���
unsigned _stdcall TcpServerNet::RecvThread(void* lpVoid) {
    TcpServerNet* pThis = (TcpServerNet*)lpVoid;
    pThis->RecvData();
    return 0;
}

//�ر����磺�˳��̡߳��رվ�����ر�socket��ж�ؿ�
void TcpServerNet::UninitNet() {
    //1���˳��߳�
    m_isStop = true;
    //ʹ�õ���������
    for (auto ite = m_listThreadHandle.begin(); ite != m_listThreadHandle.end();) {
        if (*ite) {
            if (WAIT_TIMEOUT == WaitForSingleObject(*ite, 100)) {
                //��100msʱ���ڣ��߳�û���˳�����ǿ��ɱ��
                TerminateThread(*ite, -1);

            }
            //2�����վ��
            CloseHandle(*ite);
            *ite = NULL;
        }
        ite = m_listThreadHandle.erase(ite);//erase()�Ƴ���Ч�ڵ㣬����ֵ����һ����Ч�ڵ㣨���Բ���Ҫ++��
    }
    
    // 3���ر�socket��n+1��map�е�n���͵�����m_sockListen��
    if (m_sockListen && m_sockListen != INVALID_SOCKET) {//���ڶ��Ҳ�����Ч
        closesocket(m_sockListen);
    }
    for (auto ite = m_mapThredIdToSocket.begin(); ite != m_mapThredIdToSocket.end();) {
        if (ite->second && ite->second != INVALID_SOCKET) {
            closesocket(ite->second);
        }
        ite = m_mapThredIdToSocket.erase(ite);//ͬ��Ҳ����++
    }
    // 4��ж�ؿ�
    WSACleanup();
	}

//��������
bool TcpServerNet::SendData(long lSendIp, char* buf, int nLen) {
    //1��У�����
    if (!buf || nLen <= 0) {
        cout << "TcpServerNet::SendData parameter error" << endl;
        return false;
    }

    //2���ȷ�����С
    if (send(lSendIp, (char*)&nLen, sizeof(int), 0) <= 0) {
        return false;
    }
    //3���ٷ�����С
    if (send(lSendIp, buf, nLen, 0) <= 0) {
        return false;
    }
	return true;
	}

//��������
void TcpServerNet::RecvData() {
    //��Ϊ�̴߳��������У�����socket���뻹��Ҫһ��ʱ�䣬�����߳���������һ��
    Sleep(100);
    //1����ȡ��ǰ�̶߳�Ӧ��socket
    //��ȡ�߳�id��ͨ��id��map���ҵ���Ӧ��socket��
    unsigned int threadId = GetCurrentThreadId();//GetCurrentThreadId��ȡ��ǰ�̵߳�id
    SOCKET socket = m_mapThredIdToSocket[threadId];

    //�ж�socket�ĺϷ���
    if (!socket || INVALID_SOCKET == socket) {
        cout << "socket ���Ϸ�" << endl;
        return;
    }
    int packSize = 0;
    int nRecvNum = 0;
    int offset = 0;//�ۼƷ������ݵĴ�С
    while (!m_isStop) {
        //�Ƚ��հ���С
        nRecvNum = recv(socket, (char*)&packSize, sizeof(int), 0);
        if (nRecvNum > 0) {//������ճɹ�
            //new һ���µĿռ������հ�����
            char* packBuf = new char[packSize];
            //�ٽ��հ�������
            while (packSize) {
                nRecvNum = recv(socket, packBuf + offset, packSize, 0);
                offset += nRecvNum;
                packSize -= nRecvNum;
            }
            //�ѽ��յ������ݴ����н�����
            m_pMediator->DealData(socket, packBuf, offset);
            //offset ��Ҫ���㣬�Ա���һ��ʹ��
            offset = 0;
        }
        else if (10054==WSAGetLastError()) {//�ֶ��Ͽ����ӵ�������رյ�¼ע�ᴰ�ڣ�
            break;
        }
        else {
            cout << "TcpServerNet::RecvData recv error:" << WSAGetLastError() << endl;
            break;
        }

    }
}
