#include "ckernel.h"
#include <QDebug>
#include <useritem.h>
#include <TcpClientMediator.h>
#include <QTextCodec>   //QT里面的文本编码
#include <QMessageBox>
#include <QInputDialog>

//定义计算数组下标的宏替换公式
#define NetProtocolMap(a) m_netProtocolMap[a - _DEF_PROTOCOL_BASE - 10]

CKernel::CKernel(QObject *parent) : QObject(parent),m_id(0)
{
    //1、初始化协议数组
    setProtocolMap();

    //2、创建登录&注册界面，并显示
    m_pLoginDlg=new LoginDialog;
    m_pLoginDlg->showNormal();//showNormal显示界面

    //绑定发送注册信号的信号和槽函数    （绑定信号和槽：在new出对象之后绑定）
    connect (m_pLoginDlg,SIGNAL(SIG_registerCommit(QString,QString,QString)),
             this,SLOT(slot_registerCommit(QString,QString,QString)));

    //绑定发送登录信号的信号和槽函数
    connect (m_pLoginDlg,SIGNAL(SIG_loginCommit(QString,QString)),
             this,SLOT(slot_loginCommit(QString,QString)));

    //绑定关闭登录&注册窗口的信号和槽函数
    connect (m_pLoginDlg,SIGNAL(SIG_closeLogindlg()),
             this,SLOT(slot_closeLogindlg()));

    //3、创建好友列表窗口
    m_pMainWnd=new MyChatDialog;
    //m_pMainWnd->showNormal();

    //绑定添加好友的信号和槽函数
    connect(m_pMainWnd,SIGNAL(SIG_addFriend()),this,SLOT(slot_addFriend()));

    //绑定关闭好友列表的信号和槽函数
    connect(m_pMainWnd,SIGNAL(SIG_closeMyChatdlg()),this,SLOT(slot_closeMyChatdlg()));
    //4、创建一个中介者类
    m_pMediator=new TcpClientMediator;

    //5、绑定处理所有数据的信号和槽函数
    connect(m_pMediator,SIGNAL(SIG_ReadyData(long, char*, int)),this,SLOT(slot_ReadyData(long,char*,int)));

    //打开网络
    if(!m_pMediator->OpenNet()){
        QMessageBox::about(m_pMainWnd,"提示","打开网络失败");//about 窗口提示一下
        exit(0);
    }
}

CKernel::~CKernel(){
    qDebug()<<__func__;
    //1、回收好友列表窗口
    if(m_pMainWnd){
        m_pMainWnd->hide();//先隐藏
        delete m_pMainWnd;
        m_pMainWnd=NULL;
    }
    //2、回收中介者类
    if(m_pMediator){
        m_pMediator->CloseNet();//先隐藏
        delete m_pMediator;
        m_pMediator=NULL;
    }

    //3、回收登录&注册界面
    //界面都是先隐藏再delete
    if(m_pLoginDlg){
        m_pLoginDlg->hide();
        delete m_pLoginDlg;
        m_pLoginDlg=NULL;
    }

    //4、回收map中的聊天窗口
    for(auto ite=m_mapIdToChatdlg.begin();ite!=m_mapIdToChatdlg.end();){
        //回收节点中的窗口指针
        ChatDialog*chat=*ite;
        if(chat){
            chat->hide();
            delete chat;
            chat=NULL;
        }
        //移除无效节点，返回值是下一个有效节点,所以for循环中的ite不用++
        ite=m_mapIdToChatdlg.erase(ite);
    }

}

//初始化协议数组
void CKernel::setProtocolMap(){
    qDebug()<<__func__;
    //1、初始化数组
    memset(m_netProtocolMap, 0, sizeof(pfun)*_DEF_TCP_PROTOCOL_COUNT);

    //2、绑定协议头和处理函数（把处理函数的地址放在对应的下标里）
    NetProtocolMap(_DEF_PROTOCOL_TCP_REGISTER_RS) = &CKernel::dealRegisterRs;
    NetProtocolMap(_DEF_PROTOCOL_TCP_LOGIN_RS) = &CKernel::dealLoginRs;
    NetProtocolMap(_DEF_PROTOCOL_TCP_FRIEND_INFO) = &CKernel::dealFriendInfoRq;
    NetProtocolMap(_DEF_PROTOCOL_TCP_CHAT_RS) = &CKernel::dealChatRs;
    NetProtocolMap(_DEF_PROTOCOL_TCP_CHAT_RQ) = &CKernel::dealChatRq;
    NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RQ) = &CKernel::dealAddFriendRq;
    NetProtocolMap(_DEF_PROTOCOL_TCP_ADD_FRIEND_RS) = &CKernel::dealAddFriendRs;
    NetProtocolMap(_DEF_PROTOCOL_TCP_OFFLINE_RQ) = &CKernel::dealOfflineRq;

}
//utf-8(QString) 转 gb2312(char*)   buBuf 是输出参数，传入的是要写入转码后数据的空间的起始地址，nLen 是空间长度，utf8是未转码前的数据
void CKernel::utf8ToGb2312(char *gbBuf, int nLen, QString utf8)
{
    QTextCodec* gb2312Code=QTextCodec::codecForName("gb2312");
    QByteArray ba=gb2312Code->fromUnicode(utf8);
    strcpy_s(gbBuf,nLen,ba.data());

}

//gb2312转utf-8  返回值是转码后的utf-8编码格式的数据，gbBuf是gb2312编码的字符串
QString CKernel::gb2312ToUtf8(char *gbBuf)
{
    QTextCodec* gb2312Code=QTextCodec::codecForName("gb2312");
    return gb2312Code->toUnicode(gbBuf);
}

//处理上线回复、注册
void CKernel::dealRegisterRs(long lSendIp, char* buf, int nLen){
    qDebug()<<__func__;
    //1、拆包
    STRU_REGISTER_RS* rs=(STRU_REGISTER_RS*)buf;

    //2、根据注册结果显示提示信息
    switch(rs->result){
        case register_success:
            QMessageBox::about(m_pLoginDlg,"提示","注册成功");
        break;
        case name_is_exist:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败，昵称已被使用");
        break;
        case tel_is_exist:
            QMessageBox::about(m_pLoginDlg,"提示","注册失败，电话号码已被使用");
        break;
        default:
        break;

    }

    //3、
}
//处理登录回复
void CKernel::dealLoginRs(long lSendIp, char* buf, int nLen){
    qDebug()<<__func__;
    //1、 拆包
    STRU_LOGIN_RS* rs=(STRU_LOGIN_RS*)buf;
    //2、根据登录结果回复结果显示提示内容
    switch(rs->result){
        case login_success:{
            //QMessageBox::about(m_pLoginDlg,"提示","登录成功");  不需要提示登录成功，直接跳到好友界面窗口
            //登录成功，保存当前登录用户的id，隐藏登录窗口，显示好友列表窗口
            m_id=rs->userId;
            qDebug()<<"userId:"<<m_id;
            m_pLoginDlg->hide();
            m_pMainWnd->showNormal();

        }
        break;
        case password_error:
            QMessageBox::about(m_pLoginDlg,"提示","登录失败，密码错误");
        break;
        case user_not_exist:
            QMessageBox::about(m_pLoginDlg,"提示","登录失败，用户不存在");
        break;
        default:
        break;

    }
}
//处理好友信息请求
void CKernel::dealFriendInfoRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_FRIEND_INFO*info=(STRU_FRIEND_INFO*) buf;
    QString nameTemp=gb2312ToUtf8(info->name);
    QString feelingTemp=gb2312ToUtf8(info->feeling);


    //2、判断是不是自己
    if(info->userId==m_id){
        //保存自己的信息
        m_name=nameTemp;
        //是自己的信息，就设置到界面上
        m_pMainWnd->setUserInfo(nameTemp,feelingTemp,info->iconId);
        return ;

    }
    //3、是好友信息，先判断这个好友是否已经存在
    if(m_mapIdToUseritem.count(info->userId)==0){
        //4、好友不存在，就new一个useritem
        UserItem*item=new UserItem;
        //5、设置useritem的控件属性
        item->setInfo(info->userId,info->state,info->iconId,nameTemp,feelingTemp);
        //6、把这个useritem添加到列表上，mychatdialog上
        m_pMainWnd->addFriend(item);
        //7、TODO:创建一个与该好友聊天的窗口
        ChatDialog* chat=new ChatDialog;

        //8、设置聊天窗口
        chat->setInfo(nameTemp,info->userId);

        //9、绑定聊天窗口发送聊天内容的信号和槽函数
        connect(chat,SIGNAL(SIG_sendMsg(QString,int)),this,SLOT(slot_sendMsg(QString,int)));

        //10、把聊天窗口放到map中保存
        m_mapIdToChatdlg[info->userId]=chat;

        //绑定点击useritem显示聊天窗口的信号和槽函数
        connect(item,SIGNAL(SIG_userClicked(int)),this,SLOT(slot_userClicked(int)) );//参数：谁发出，信号，谁接收，槽函数

        //11、把useritem放到map中保存
        m_mapIdToUseritem[info->userId]=item;
    }
    else{
        //好友已经在列表中了---在上线时，好友本来是下线状态，然后上线
        //12、取出已经存在的useritem
        UserItem* item=m_mapIdToUseritem[info->userId];
        //13、重新设置控件的属性
        item->setInfo(info->userId,info->state,info->iconId,nameTemp,feelingTemp);
    }
}

//处理聊天回复（服务端回复客户端的请求为不在线，处理不在线情况）
void CKernel::dealChatRs(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_TCP_CHAT_RS*rs=(STRU_TCP_CHAT_RS*)buf;

    //2、找到聊天窗口
    if(m_mapIdToChatdlg.count(rs->friendId)>0){
        ChatDialog*chat=m_mapIdToChatdlg[rs->friendId];
        //3、设置好友不在线到聊天窗口上
        chat->setFriendOffline();
    }


}

//处理聊天请求（服务端回复客户端的请求为发送的信息，处理在线情况）
void CKernel::dealChatRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_TCP_CHAT_RQ*rq=(STRU_TCP_CHAT_RQ*)buf;

    //2、找到聊天窗口
    if(m_mapIdToChatdlg.count(rq->userId)>0){
        ChatDialog*chat=m_mapIdToChatdlg[rq->userId];
        //3、设置聊天内容到窗口，并显示窗口
        chat->setContent(rq->content);
        chat->showNormal();
    }
}

//处理添加好友请求(A添加B好友，此时是B客户端，收到A客户端的请求)
void CKernel::dealAddFriendRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_ADD_FRIEND_RQ*rq=(STRU_ADD_FRIEND_RQ*)buf;
    STRU_ADD_FRIEND_RS rs;
    //2、弹出一个提示窗口，是否同意添加XXX为好友
    QString str=QString("用户【%1】请求添加你为好友，是否同意？").arg(rq->userName);
    if(QMessageBox::Yes==QMessageBox::question(m_pMainWnd,"添加好友",str)){//点同意
        //同意添加好友
        rs.result=add_success;
    }else{
        //拒绝添加好友请求
        rs.result=user_refuse;
    }

    //3、返回给服务器添加结果（同意/拒绝）
    rs.userId=rq->userId;
    rs.friendId=m_id;
    strcpy(rs.friendName,m_name.toStdString().c_str());
    m_pMediator->SendData(0,(char*)&rs,sizeof(rs));

}

//处理添加好友回复(A添加B好友，此时是A客户端，收到B客户端的回复)
void CKernel::dealAddFriendRs(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_ADD_FRIEND_RS*rs=(STRU_ADD_FRIEND_RS*)buf;
    QString friendName=gb2312ToUtf8(rs->friendName);
    //2、根据回复结果，显示提示信息
    switch(rs->result) {
    case add_success:
        QMessageBox::about(m_pMainWnd,"提示",QString("添加好友【%1】成功").arg(rs->friendName));
        break;
    case no_this_user:
        QMessageBox::about(m_pMainWnd,"提示",QString("添加好友【%1】失败，好友不存在").arg(friendName));
        break;
    case user_offline:
        QMessageBox::about(m_pMainWnd,"提示",QString("添加好友【%1】失败，好友不在线").arg(friendName));
        break;
    case user_refuse:
        QMessageBox::about(m_pMainWnd,"提示",QString("【%1】拒绝添加你为好友").arg(rs->friendName));
        break;
    default:
        break;
    }
}

//处理下线请求
void CKernel::dealOfflineRq(long lSendIp, char *buf, int nLen)
{
    qDebug()<<__func__;
    //1、拆包
    STRU_TCP_OFFLINE_RQ*rq=(STRU_TCP_OFFLINE_RQ*)buf;
    //2、找到下线好友的useritem，设置为离线状态
    if(m_mapIdToUseritem.count(rq->userId)>0){
        UserItem*item=m_mapIdToUseritem[rq->userId];
        item->setUserOffline();
    }
}


//处理所有接收到的数据
 void CKernel::slot_ReadyData(long lSendIp, char* buf, int nLen){
     qDebug()<<__func__;//打印一下函数名字
     //1、取出协议头
     int type =*(int*)buf;
     //2、根据协议头判断是哪个结构体，走对应的处理流程
     //2.1、判断协议头是否在有效值范围内
           if((type - _DEF_PROTOCOL_BASE - 10)>= 0&&
           (type - _DEF_PROTOCOL_BASE -10)<(_DEF_TCP_PROTOCOL_COUNT)){
           //2.2、通过计算数组下标，取出函数地址
           pfun pf=NetProtocolMap(type);
           // 2.3、调用函数
           if(pf){
           (this->*pf)(lSendIp,buf,nLen);
           }else {
           qDebug()<< "type:"<<type;
            }
 }
           //3、处理完数据，回收全间
                   delete[] buf;

 }
 //把注册信号发给kernel
 void CKernel::slot_registerCommit(QString tel, QString name, QString password)
 {
     qDebug()<<__func__;
     qDebug()<<"tel:"<<tel<<",name:"<<name<<",password:"<<password;
     //1、打包（把数据放在结构体里面）
     STRU_REGISTER_RQ rq;
     strcpy(rq.tel,tel.toStdString().c_str());//放char数组里
     //strcpy(rq.name,name.toStdString().c_str());
     utf8ToGb2312(rq.name,sizeof(rq.name),name);//将输入的昵称转为gb2312。并放到rq.name中
     strcpy(rq.password,password.toStdString().c_str());

     //2、通过中介者类把数据发送给服务器
     m_pMediator->SendData(0,(char*)&rq,sizeof(rq));//参数0是随便写的
 }
  //把登录信号发给kernel
 void CKernel::slot_loginCommit(QString tel, QString password)
 {
     qDebug()<<__func__;
     qDebug()<<"tel:"<<tel<<",password:"<<password;
     //1、打包
     STRU_LOGIN_RQ rq;
     strcpy(rq.tel,tel.toStdString().c_str());//strcpy()把Qtring类型变量放char数组里
     strcpy(rq.password,password.toStdString().c_str());

     //2、通过中介者类把数据发送给服务器
     m_pMediator->SendData(0,(char*)&rq,sizeof(rq));//参数0是随便写的

 }

 //处理显示与好友的聊天窗口的信号
 void CKernel::slot_userClicked(int id)
 {
     qDebug()<<__func__;
     qDebug()<<"id:"<<id;
     //判断id对应的窗口是否存在
     if(m_mapIdToChatdlg.count(id)>0){
         //取出聊天窗口，并显示
         ChatDialog* chat=m_mapIdToChatdlg[id];
         chat->showNormal();
     }
 }

 //处理聊天内容和id
 void CKernel::slot_sendMsg(QString content, int id)
 {
     qDebug()<<__func__;
     qDebug()<<"id:"<<id<<",content:"<<content;
     //1、打包
     STRU_TCP_CHAT_RQ rq;
     rq.userId=m_id;
     rq.friendId=id;
     //聊天内容不需要转码，因为服务器只负责转发聊天内容，不处理聊天内容
     strcpy(rq.content,content.toStdString().c_str());

     //2、发送给服务端
     m_pMediator->SendData(0,(char*)&rq,sizeof(rq));

 }
 //处理添加好友的信号
 void CKernel::slot_addFriend()
 {
     qDebug()<<__func__;
     //1、弹出一个窗口，让用户输入昵称---我们这里只通过昵称添加好友
     QString friendName=QInputDialog::getText(m_pMainWnd,"添加好友","请输入好友的昵称:");//头文件<QInputDialog>
     //getText参数：父窗口(mychatdialog)，标题，text内容

     QString friendNameTemp=friendName;
     //2.1、校验用户输入的数据是否合法
     if(friendName.isEmpty()||friendNameTemp.remove(" ").isEmpty()){
         QMessageBox::about(m_pMainWnd,"提示","输入为空或者空字符串");
         return;
     }

     //2.2、校验长度，昵称不能超过10
     if(friendName.length()>10){
     QMessageBox::about(m_pMainWnd,"提示","昵称不能超过10");
     return;}


     //3、输入的昵称是不是自己的昵称
     if(friendName==m_name){//自己的昵称在登录成功的时候保存（上面处理好友信息请求dealFriendInfoRq函数中）
         QMessageBox::about(m_pMainWnd,"提示","不能添加自己为好友");
         return ;
     }

     //4、输入的昵称是不是已经是好友了
     for(auto ite=m_mapIdToUseritem.begin();ite!=m_mapIdToUseritem.end();ite++){
         UserItem* item=*ite;
         if(friendName==item->m_name){//好友昵称
             QMessageBox::about(m_pMainWnd,"提示","已是好友，无需重复添加");
             return;
         }
     }

     //5、以上都不满足，就给服务端发送添加好友请求
     STRU_ADD_FRIEND_RQ rq;
     rq.userId=m_id;
     strcpy(rq.userName,m_name.toStdString().c_str());//转成char*
     //strcpy(rq.friendName,friendName.toStdString().c_str());
     //写服务端时会发现，需要将昵称取出来，用来判断是否在线，所以这里的friendName需要转码
     utf8ToGb2312(rq.friendName,sizeof(rq.friendName),friendName);
     m_pMediator->SendData(0,(char*)&rq,sizeof(rq));
 }

 //处理关闭登录&注册窗口的信号（为什么有析构函数还要写这个？因为，关闭窗口时不走析构函数）
 //（为什么不发给服务器处理？因为此时还没登录，是自己的事情）
 void CKernel::slot_closeLogindlg()
 {
     qDebug()<<__func__;
     //1、回收资源
     if(m_pMainWnd){//窗口回收之前要先隐藏
         m_pMainWnd->hide();//先隐藏
         delete m_pMainWnd;//再delete
         m_pMainWnd=NULL;//最后置空
     }
    //2、回收中介者类
     if(m_pMediator){
         m_pMediator->CloseNet();//中介者类delete之前先关闭网络
         delete m_pMediator;
         m_pMediator=NULL;
     }
    //3、回收登录&注册界面
     if(m_pLoginDlg){
         m_pLoginDlg->hide();
         delete m_pLoginDlg;
         m_pLoginDlg=NULL;
     }
    //4、回收map中的聊天
     //m_mapIdToUseritem不用在这里回收，因为每一个new出来的useritem都放在mychatdialog上，
     //所以useritem的回收会由在回收mychatdialog QT会自动回收。但ChatDialog不一样，需要自己回收
     for(auto ite=m_mapIdToChatdlg.begin();ite!=m_mapIdToChatdlg.end();){
         //回收节点中的窗口指针
         ChatDialog*chat=*ite;
         if(chat){
             chat->hide();
             delete chat;
             chat=NULL;
         }
         //移除无效节点，返回值是下一个有效节点,所以for循环中的ite不用++
         ite=m_mapIdToChatdlg.erase(ite);
     }

     //2、结束进程
     exit(0);

 }

 //处理关闭好友列表界面的信号
 void CKernel::slot_closeMyChatdlg()
 {
     qDebug()<<__func__;
     //1、给服务器发送下线请求
     STRU_TCP_OFFLINE_RQ rq;
     rq.userId=m_id;
     m_pMediator->SendData(0,(char*)&rq,sizeof(rq));

     //2、回收资源、结束进程
     slot_closeLogindlg();

 }
