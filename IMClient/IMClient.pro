#-------------------------------------------------
#
# Project created by QtCreator 2024-07-23T09:27:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IMClient
TEMPLATE = app

INCLUDEPATH +=./net
INCLUDEPATH +=./mediator

LIBS +=-lWs2_32

SOURCES += main.cpp\
        mychatdialog.cpp \
    net/TcpClientNet.cpp \
    mediator/TcpClientMediator.cpp \
    mediator/INetMediator.cpp \
    ckernel.cpp \
    logindialog.cpp \
    useritem.cpp \
    chatdialog.cpp


HEADERS  += mychatdialog.h \
    net/INet.h \
    net/packDef.h \
    net/TcpClientNet.h \
    mediator/INetMediator.h \
    mediator/TcpClientMediator.h \
    ckernel.h \
    logindialog.h \
    useritem.h \
    chatdialog.h


FORMS    += mychatdialog.ui \
    logindialog.ui \
    useritem.ui \
    chatdialog.ui

RESOURCES += \
    resource.qrc
