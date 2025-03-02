#ifndef MYCHATDIALOG_H
#define MYCHATDIALOG_H

#include <QMenu>
#include <QDialog>
#include <QVBoxLayout>
#include <QCloseEvent>
#include "useritem.h"

namespace Ui {
class MyChatDialog;
}

class MyChatDialog : public QDialog
{
    Q_OBJECT
signals:
    //添加好友的信号
    void SIG_addFriend();
    //通知kernel关闭好友列表界面
    void SIG_closeMyChatdlg();

public:
    explicit MyChatDialog(QWidget *parent = 0);
    ~MyChatDialog();
    //重写关闭窗口事件
    void closeEvent(QCloseEvent* event);
    //添加好友
    void addFriend(UserItem* item);
    //删除好友
    void deleteFriend(UserItem* item);
    //设置控件属性
    void setUserInfo(QString name,QString feeling,int iconId);
private slots:
    void on_pb_menu_clicked();
    //处理菜单项的点击事件
    void slot_dealMenu(QAction* action);
private:
    Ui::MyChatDialog *ui;
    //垂直布局的层  <QVBoxLayout>
    QVBoxLayout* m_layout;
    //菜单
    QMenu* m_menu;//头文件<QMenu>

};

#endif // MYCHATDIALOG_H
