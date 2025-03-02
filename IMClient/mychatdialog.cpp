#include "mychatdialog.h"
#include "ui_mychatdialog.h"
#include <QDebug>
#include <QMessageBox>

MyChatDialog::MyChatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyChatDialog)
{
    ui->setupUi(this);
    setWindowTitle("IM QQ");//设置标题

    //初始化垂直布局的层(可以理解为画布，小控件useritem放在上面)
    m_layout=new QVBoxLayout;

    //设置距离外控件上下左右的间距
    m_layout->setContentsMargins(0,0,0,0);

    //设置里面每个控件彼此间的距离
    m_layout->setSpacing(3);//肉眼基本上看不见

    //把层设置到外部的控件上
    ui->tw_list->setLayout(m_layout);

    //添加菜单
    m_menu=new QMenu(this);//参数：父窗口是下面大的窗口mychatdialog窗口（this这里指的就是当前写的这个mychatdialog窗口）
    //往菜单里添加菜单项
    m_menu->addAction("系统设置");
    m_menu->addAction("添加好友");//如果想添加更多功能继续在这添加
    //绑定点击菜单项的信号和槽函数
    connect(m_menu,SIGNAL(triggered(QAction*)),
            this,SLOT(slot_dealMenu(QAction*)));//信号是menu点击时自动发的，但是槽函数是自己实现的，所以需要绑定一下，才知道当发送信号时，执行的是哪个槽函数
}

MyChatDialog::~MyChatDialog()
{
    delete ui;
    //回收资源
    if(m_layout){
        delete m_layout;
        m_layout=NULL;
    }
    if(m_menu){
        delete m_menu;
        m_menu=NULL;
    }
}

//重写关闭窗口事件
void MyChatDialog::closeEvent(QCloseEvent *event)
{
    //忽略关闭事件 -> 不走父类的关闭窗口的流程
    event->ignore();//如果没有这条语句，不管点Yes或No都会调用~CKernel关闭窗口，退出
    //弹出确认关闭窗口询问用户
    if(QMessageBox::Yes==QMessageBox::question(this,"提示","是否确认退出？")){
        //1、回收类里的资源
        if(m_layout){
            delete m_layout;
            m_layout=NULL;
        }
        if(m_menu){
            delete m_menu;
            m_menu=NULL;
        }
        //2、通知kernel关闭应用程序
        Q_EMIT SIG_closeMyChatdlg();
    }

}

//添加好友
void MyChatDialog::addFriend(UserItem *item)
{
    //往层上添加控件
    m_layout->addWidget(item);

}

//删除好友
void MyChatDialog::deleteFriend(UserItem *item)
{

}

//设置控件属性（用户自己）
void MyChatDialog::setUserInfo(QString name, QString feeling, int iconId)
{
    ui->lb_name->setText(name);//通过ui找到控件,然后调用控件对应的函数
    ui->le_feeling->setText(feeling);
    ui->pb_icon->setIcon(QIcon(QString(":/tx/%1.jpg").arg(iconId)) );
}

//显示菜单列表
void MyChatDialog::on_pb_menu_clicked()
{
    //获取当前鼠标点击位置
    QPoint p=QCursor::pos();//QCursor鼠标 pos位置

    //计算菜单项的高度，sizeHint是绝对高度，也就是所以菜单项显示出来以后的整个菜单的高度
    QSize size=m_menu->sizeHint();

    //在点击的位置向上显示一个菜单列表
    //m_memu->exec(p);  //这样是从点击点（点击点在菜单的左上方）向下显示，如果想让其向上显示（点击点在菜单的左下方），x值不变，y值向上n个菜单项的宽度即可，也就是整个菜单的绝对高度
    m_menu->exec(QPoint(p.x(),p.y()-size.height()));
}

//处理菜单项的点击事件
void MyChatDialog::slot_dealMenu(QAction *action)
{
    if("添加好友"==action->text()){
        //给kernel发消息，通知kernel要添加好友
        Q_EMIT SIG_addFriend();
    }else if("系统设置"==action->text()){
        qDebug()<<"系统设置";//没实现
    }
}
