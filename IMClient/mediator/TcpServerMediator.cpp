#include "TcpServerMediator.h"
#include "TcpServerNet.h"

TcpServerMediator::TcpServerMediator() {
	m_pNet = new TcpServerNet(this);
}
TcpServerMediator::~TcpServerMediator() {
	if (m_pNet) {
		delete m_pNet;
		m_pNet = NULL;
	}

}
//打开网络
bool TcpServerMediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		cout << "TcpServerMediator::OpenNet() fail" << endl;
		return false;
	}
	return true;

}
//关闭网络
void TcpServerMediator::CloseNet() {
	m_pNet->UninitNet();
}
//发送数据
bool TcpServerMediator::SendData(long lSendIp, char* buf, int nLen) {
	if (!m_pNet->SendData(lSendIp, buf, nLen)) {
		cout << "TcpServerMediator::SendData fail" << endl;
		return false;
	}
	return true;
}
//接收数据
void TcpServerMediator::DealData(long lSendIp, char* buf, int nLen) {
	//TODO: 把接收到的数据传给kernel类
	cout << "TcpServerMediator::DealData" << buf << endl;
	//再把数据传回给客户端
	SendData(lSendIp, buf, nLen);
}