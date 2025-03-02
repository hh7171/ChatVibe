#include "TcpServerMediator.h"
#include "TcpServerNet.h"
#include "../CKernel.h"//../指往上一层
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
	//把接收到的数据传给kernel类
	CKernel::pKernel->dealData(lSendIp, buf, nLen);
	//怎么将数据传给kernel：要有一个CKernel的对象，通过对象调用dealData()函数。
	//但是，在CKernel类中已经有中介者类的对象了，如果直接new一个CKernel类，就有“我中有你，你中有我”的错误。
	//应该：用静态成员变量（有类似全局变量的作用，在哪都可以用）
	//定义一个CKernel类的静态的全局的成员变量（pKernel）,指向new的CKernel对象，这样以来，在中介者中可以使用该指针调用CKernel类的函数
}