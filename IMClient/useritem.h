#ifndef USERITEM_H
#define USERITEM_H

#include <QWidget>

namespace Ui {
class UserItem;
}

class UserItem : public QWidget
{
    Q_OBJECT

signals:
    //发送信号给kernel显示与好友的聊天窗口
    void SIG_userClicked(int id);

public:
    explicit UserItem(QWidget *parent = 0);
    ~UserItem();
    //保存用户信息和设置控件属性
    void setInfo(int id,int state,int iconId,QString name,QString feeling);
    //设置用户为离线状态
    void setUserOffline();
private slots:
    void on_pb_icon_clicked();

    void on_pb_icon_clicked(bool checked);
public:
    QString m_name;
private:
    Ui::UserItem *ui;
    //保存用户信息
    int m_id;
    int m_state;
    int m_iconId;
    QString m_feeling;
};

#endif // USERITEM_H
