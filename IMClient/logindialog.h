#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

signals:
    //把注册信号发给kernel
    void SIG_registerCommit(QString tel,QString name,QString password);
    //把登录信号发给kernel
    void SIG_loginCommit(QString tel,QString password);
    //通知kernel关闭登录&注册窗口
    void SIG_closeLogindlg();

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    //重写父类的关闭窗口事件  logindialog.ui界面点叉号能退出是因为，LoginDialog类继承与QDialog父类，父类中自带退出的功能，但是
    //并不能将LoginDialog类中的资源并没有回收，这样就会导致资源泄漏的问题，所以我们需要重写父类关闭窗口对应的函数，同理mychatdialog窗口也是
    void closeEvent(QCloseEvent * event);//头文件<QCloseEvent>
private slots:
    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
