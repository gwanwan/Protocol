#-------------------------------------------------
#
# Project created by QtCreator 2013-09-03T14:44:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myTcpClient
TEMPLATE = app

DEFINES += BOOST_THREAD_USE_LIB

INCLUDEPATH += $$PWD/../../boost_1_59_0/boost_1_59_0
DEPENDPATH += $$PWD/../../boost_1_59_0/boost_1_59_0

#LIBS += C:\Program Files\CryptoPP\lib\Debug\cryptlib.lib
LIBS += E:\boost_1_59_0\boost_1_59_0\lib\libboost_serialization-mgw53-mt-sd-1_59.a

QT += multimedia
SOURCES += main.cpp\
        frmmain.cpp \
    aes.cpp \
    protocol.cpp

HEADERS  += frmmain.h \
    myhelper.h \
    aes.h \
    protocols.h \
    protocol.h

FORMS    += frmmain.ui

MOC_DIR=temp/moc
RCC_DIR=temp/rcc
UI_DIR=temp/ui
OBJECTS_DIR=temp/obj
DESTDIR=bin
