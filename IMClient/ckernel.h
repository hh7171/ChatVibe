#ifndef CKERNEL_H
#define CKERNEL_H

#include <QMap>
#include <QObject>
#include "packDef.h"
#include "logindialog.h"
#include "chatdialog.h"
#include "mychatdialog.h"
#include "INetMediator.h"
#include "useritem.h"

//定义函数指针
class CKernel;
typedef void(CKernel::*pfun)(long,char*,int);

class CKernel : public QObject
{
    Q_OBJECT
public:
    explicit CKernel(QObject *parent = 0);
    ~CKernel();

private:
    //初始化协议数组
    void setProtocolMap();
    //utf-8(QString) 转 gb2312(char*)  gbBuf 是输出参数，传入的是要写入转码后数据的空间的起始地址，nLen 是空间长度，utf8是未转码前的数据
    static void utf8ToGb2312(char* gbBuf,int nLen,QString utf8);

    //gb2312转utf-8   返回值是转码后的utf-8编码格式的数据，gbBuf是gb2312编码的字符串
    static QString gb2312ToUtf8(char* gbBuf);
    //处理上线回复
    void dealRegisterRs(long lSendIp, char* buf, int nLen);
    //处理登录回复
    void dealLoginRs(long lSendIp, char* buf, int nLen);
    //处理好友信息请求
    void dealFriendInfoRq(long lSendIp, char* buf, int nLen);
    //处理聊天回复
    void dealChatRs(long lSendIp, char* buf, int nLen);
    //处理聊天请求
    void dealChatRq(long lSendIp, char* buf, int nLen);
    //处理添加好友请求
    void dealAddFriendRq(long lSendIp, char* buf, int nLen);
    //处理添加好友回复
    void dealAddFriendRs(long lSendIp, char* buf, int nLen);
    //处理下线请求
    void dealOfflineRq(long lSendIp, char* buf, int nLen);
signals:


public slots:
    //处理所有接收到的数据
     void slot_ReadyData(long lSendIp, char* buf, int nLen);
    //把注册信号发给kernel
    void slot_registerCommit(QString tel,QString name,QString password);
    //把登录信号发给kernel
    void slot_loginCommit(QString tel,QString password);
    //处理显示与好友的聊天窗口的信号
    void slot_userClicked(int id);
    //处理聊天内容和id
    void slot_sendMsg(QString content, int id);
    //处理添加好友的信号
    void slot_addFriend();
    //处理关闭登录&注册窗口的信号
    void slot_closeLogindlg();
    //处理关闭好友列表界面的信号
    void slot_closeMyChatdlg();
private:
    int m_id;//用户id
    QString m_name;
    MyChatDialog* m_pMainWnd;
    INetMediator* m_pMediator;
    LoginDialog* m_pLoginDlg;
    //定义个数组，下标是协议头计算的，数组的内容是函数指针
    pfun m_netProtocolMap[_DEF_TCP_PROTOCOL_COUNT];//通过数组来存储不同函数，利用数组下标与协议头特点

    //把useritem放在map中保存，key是Id
    QMap<int,UserItem*> m_mapIdToUseritem;//里面存储的小控件是放在大控件mychatdialog上的，所以小控件的回收由大控件负责，在回收大控件的时候，QT会自动回收小控件

    //把聊天窗口放到map中保存，key是id
    QMap<int,ChatDialog*> m_mapIdToChatdlg;//里面存储的ChatDialog是一个独立的窗口，需要手动遍历回收（在ckernel.cpp中）
};

#endif // CKERNEL_H
