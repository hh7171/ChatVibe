#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "QTime"

ChatDialog::ChatDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatDialog)
{
    ui->setupUi(this);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}
//设置窗口信息
void ChatDialog::setInfo(QString name, int id){
    // 保存成员变量
    m_name = name;
    m_id = id;

    //设置窗口的title
    setWindowTitle(QString("与【%1】的聊天窗口").arg(m_name));
}

//设置聊天内容到窗口上
void ChatDialog::setContent(QString content){
    // 3、显示输入内容到浏览窗口，显示格式：【昵称】 时间（换行）内容
    ui->tb_chat->append(QString("【%1】%2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
}
//设置好友不在线到窗口上
void ChatDialog::setFriendOffline()
{
    //显示输入内容到浏览窗口，显示格式：【昵称】 时间 好友不在线
    ui->tb_chat->append(QString("【%1】%2").arg(m_name).arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(QString("好友不在线"));

}
void ChatDialog::on_pb_send_clicked(){
    //1、取出用户输入的数据，校验
    QString content = ui->te_chat->toPlainText(); // 不带格式的纯文本
    if (content.isEmpty()){
    return;
    }
    // 2、清空编辑窗口
    content =ui->te_chat->toHtml(); // 带格式的
    ui->te_chat->clear();
    //3、显示输入内容到浏览窗口，显示格式： 【我】时间（换行）内容
    ui->tb_chat->append(QString("【我】 %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    // 4、发送聊天内容和id地址给kernel
    Q_EMIT SIG_sendMsg(content,m_id);
}
