#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QWidget>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QWidget
{
    Q_OBJECT
signals:
    //发送聊天内容和id给kernel
    void SIG_sendMsg(QString content, int id);
public:
    explicit ChatDialog(QWidget *parent = 0);
    ~ChatDialog();
    //设置窗口信息
    void setInfo(QString name, int id);
    // 设置聊天内容到窗口上
    void setContent(QString content);
    //设置好友不在线到窗口上
    void setFriendOffline();

private slots:
    void on_pb_send_clicked();
private:
    Ui::ChatDialog *ui;
    QString m_name;
    int m_id;
};

#endif // CHATDIALOG_H
