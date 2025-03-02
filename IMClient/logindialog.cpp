#include "logindialog.h"
#include "ui_logindialog.h"
#include <QDebug>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowTitle("IM QQ");//设置title
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

//重写父类的关闭窗口事件
void LoginDialog::closeEvent(QCloseEvent *event)
{
    //1、回收类里的资源 (当前类里没有资源了，logindialog.h中我们看到只有一个LoginDialog *ui，而且在析构函数中我们delete了)

    //2、通知kernel关闭应用程序（类自己的资源自己负责回收，类与类之间相互提醒回收-QT中使用信号）
    Q_EMIT SIG_closeLogindlg();
}

void LoginDialog::on_pb_clear_clicked()
{
    ui->le_tel->setText("");//设置内容为空
    ui->le_password->setText("");
}

void LoginDialog::on_pb_commit_clicked()
{
    qDebug()<<__func__;
    //1、从控件上获取输入的数据
    QString tel=ui->le_tel->text();//获取文本内容
    QString password=ui->le_password->text();
    qDebug()<<"tel:"<<tel<<",password:"<<password;

    //2、校验用户输入的数据
    //校验规则：
    //2.1、不能为空，不能只包含空格(用remove函数，这个函数会删除空格，这样再检测remove之后的是不是空就可以检测出是不是全为空格，
    //但是，remove函数是直接在上面进行删除空格的，这个remove之后的数据就不是原来的数据了，所以我们要提前保存一个副本，用副本来检验)
    QString telTemp=tel;//副本
    QString passwordTemp=password;
    if(tel.isEmpty()||password.isEmpty()||
       telTemp.remove(" ").isEmpty()||passwordTemp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入为空或者空字符串");
        return;
    }

    //2.2、校验长度，手机号必须是11位，昵称不能超过10，密码不能超过15
    if(11!=tel.length()||password.length()>15){
    QMessageBox::about(this,"提示","手机号必须是11位，昵称不能超过10，密码不能超过15");
    return;}

    //2.3、校验内容，手机号必须满足一定规则的数字—使用正则表达式
    QRegularExpression regex("^1([358][0-9]|4[579]|6[2567]|7[0135678]|9[0123589])[0-9]{8}$"); // 手机号正则表达式
     QRegularExpressionMatch match = regex.match(tel);
     if (!match.hasMatch()) {
         QMessageBox::about(this, "提示", "手机号格式不正确");
         return;
     }


    //3、通过信号把数据发给kernel
    Q_EMIT SIG_loginCommit(tel,password);//Q_EMIT 发送信号

}

void LoginDialog::on_pb_clear_register_clicked()
{
    ui->le_tel_register->setText("");
    ui->le_password_register->setText("");
    ui->le_name_register->setText("");
}

void LoginDialog::on_pb_commit_register_clicked()
{
    qDebug()<<__func__;
    //1、从控件上获取输入的数据
    QString tel=ui->le_tel_register->text();
    QString name=ui->le_name_register->text();
    QString password=ui->le_password_register->text();
    qDebug()<<"tel:"<<tel<<",name:"<<name<<",password:"<<password;//将数据显示一遍

    //2、校验用户输入的数据
    //校验规则：
    //2.1、不能为空，不能只包含空格(用remove函数，这个函数会删除空格，这样再检测remove之后的是不是空就可以检测出是不是全为空格，
    //但是，remove函数是直接在上面进行删除空格的，这个remove之后的数据就不是原来的数据了，所以我们要提前保存一个副本，用副本来检验)
    QString telTemp=tel;
    QString nameTemp=name;
    QString passwordTemp=password;
    if(tel.isEmpty()||name.isEmpty()||password.isEmpty()||
       telTemp.remove(" ").isEmpty()||nameTemp.remove(" ").isEmpty()||passwordTemp.remove(" ").isEmpty()){
        QMessageBox::about(this,"提示","输入为空或者空字符串");
        return;
    }

    //2.2、校验长度，手机号必须是11位，昵称不能超过10，密码不能超过15
    if(11!=tel.length()||name.length()>10||password.length()>15){
    QMessageBox::about(this,"提示","手机号必须是11位，昵称不能超过10，密码不能超过15");
    return;}

    //2.3、校验内容，手机号必须满足一定规则的数字—使用正则表达式
    QRegularExpression regex("^1([358][0-9]|4[579]|6[2567]|7[0135678]|9[0123589])[0-9]{8}$"); // 手机号正则表达式
     QRegularExpressionMatch match = regex.match(tel);
     if (!match.hasMatch()) {
         QMessageBox::about(this, "提示", "手机号格式不正确");
         return;
     }


    //3、通过信号把数据发给kernel
    Q_EMIT SIG_registerCommit(tel,name,password);
}
