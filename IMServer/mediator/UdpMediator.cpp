#include "UdpMediator.h"
#include"UdpNet.h"
#include<iostream>
UdpMediator::UdpMediator() {
	//m_pNet = new UdpNet;
	m_pNet = new UdpNet(this);//参数是INetMediator*的，this就代表指向这个类的指针
}
UdpMediator::~UdpMediator() {
	if (m_pNet) {
		m_pNet->UninitNet();
		delete m_pNet;
		m_pNet = NULL;
	}
}
//打开网络
bool UdpMediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
	}
	//关闭网络
void UdpMediator::CloseNet() {
	m_pNet->UninitNet();
	}
	//发送数据
bool UdpMediator::SendData(long lSendIp, char* buf, int nLen) {
	if (!m_pNet->SendData(lSendIp,buf,nLen)) {
		return false;
	}
	return true;
	}
	//接收数据
void UdpMediator::DealData(long lSendIp, char* buf, int nLen) {
	//TODO:传递给knrnel类
	std::cout<<"UdpMediator::DealData  "<<buf<<std::endl;
	}