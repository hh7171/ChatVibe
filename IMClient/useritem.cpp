#include "useritem.h"
#include "ui_useritem.h"
#include "packDef.h"
#include <QIcon>
#include <QBitmap>  //设置头像为灰色
UserItem::UserItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserItem)
{
    ui->setupUi(this);
}

UserItem::~UserItem()
{
    delete ui;
}

//保存用户信息和设置控件属性
void UserItem::setInfo(int id, int state, int iconId, QString name, QString feeling)
{
    //1、保存用户信息
    m_id=id;
    m_name=name;
    m_state=state;
    m_iconId=iconId;
    m_feeling=feeling;

    //2、设置控件属性
    //2.1、设置昵称和签名
    ui->lb_name->setText(m_name);
    ui->lb_feeling->setText(m_feeling);

    //2.2、设置头像
    //拼接头像文件路径
    QString iconPath=QString(":/tx/%1.jpg").arg(m_iconId);  //iconId.jpg
    //判断用户状态，是否在线
    if(status_online==m_state){
        //在线，亮显头像
        ui->pb_icon->setIcon(QIcon(iconPath));//参数是头像的路径，根据路径生成一个头像，放在pb_icon控件中，setIcon()是<QIcon>头文件中的函数
    }else{
        //不在线，显示灰色图标--需要用到<QBitmap>头文件
        QBitmap bmp;//定义一个QBitmap对象
        bmp.load(iconPath);//头文件<QBitmap>
        ui->pb_icon->setIcon(bmp);
    }
    //3、立即重绘
    this->repaint();

}
//设置用户为离线状态
void UserItem::setUserOffline()
{
    //1、修改用户状态
    m_state=status_offline;

    //2、设置头像为灰色
    QString iconPath=QString(":/tx/%1.jpg").arg(m_iconId);
    QBitmap bmp;//定义一个对象
    bmp.load(iconPath);
    ui->pb_icon->setIcon(bmp);

    //3、立即重绘
    this->repaint();

}

void UserItem::on_pb_icon_clicked()
{
    //给kernel发送信号，显示窗口
    Q_EMIT SIG_userClicked(m_id);
}

void UserItem::on_pb_icon_clicked(bool checked)
{

}
