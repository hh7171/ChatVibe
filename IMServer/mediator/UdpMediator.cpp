#include "UdpMediator.h"
#include"UdpNet.h"
#include<iostream>
UdpMediator::UdpMediator() {
	//m_pNet = new UdpNet;
	m_pNet = new UdpNet(this);//������INetMediator*�ģ�this�ʹ���ָ��������ָ��
}
UdpMediator::~UdpMediator() {
	if (m_pNet) {
		m_pNet->UninitNet();
		delete m_pNet;
		m_pNet = NULL;
	}
}
//������
bool UdpMediator::OpenNet() {
	if (!m_pNet->InitNet()) {
		return false;
	}
	return true;
	}
	//�ر�����
void UdpMediator::CloseNet() {
	m_pNet->UninitNet();
	}
	//��������
bool UdpMediator::SendData(long lSendIp, char* buf, int nLen) {
	if (!m_pNet->SendData(lSendIp,buf,nLen)) {
		return false;
	}
	return true;
	}
	//��������
void UdpMediator::DealData(long lSendIp, char* buf, int nLen) {
	//TODO:���ݸ�knrnel��
	std::cout<<"UdpMediator::DealData  "<<buf<<std::endl;
	}