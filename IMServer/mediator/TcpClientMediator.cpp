#include "TcpClientMediator.h"
#include "TcpClientNet.h"

TcpClientMediator::TcpClientMediator() {
	m_pNet = new TcpClientNet(this);
}
TcpClientMediator::~TcpClientMediator() {
	if (m_pNet) {//回收指针
		delete m_pNet;
		m_pNet = NULL;
	}
}
//打开网络
bool TcpClientMediator::OpenNet() {
	if (!m_pNet->InitNet()) {//调用INet类中的初始化网络函数
		cout << "TcpClientMediator::OpenNet() fail" << endl;
	}
	return true;

}
//关闭网络
void TcpClientMediator::CloseNet() {
	m_pNet->UninitNet();

}
//发送数据
bool TcpClientMediator::SendData(long lSendIp, char* buf, int nLen) {
	if (!m_pNet->SendData(lSendIp, buf,  nLen)) {
		cout << "TcpClientMediator::SendData fail" << endl;
	}
	return true;
}
//接收数据
void TcpClientMediator::DealData(long lSendIp, char* buf, int nLen) {

	//TODO: 通过信号把接收到的数据传给kernel类
	cout << "TcpClientMediator::DealData" << buf << endl;

}