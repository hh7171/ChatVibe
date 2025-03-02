#include "mychatdialog.h"
#include <QApplication>   //QT里面必须包含的头文件，应用
#include "ckernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MyChatDialog w;
//    w.show();
    CKernel kernel;
    return a.exec();//一直运行，相当于一个死循环（QT中的固定写法）
}
