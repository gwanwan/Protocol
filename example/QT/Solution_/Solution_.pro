#-------------------------------------------------
#
# Project created by QtCreator 2016-09-19T10:36:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Solution_
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../../src/command.cpp \
    ../../../src/FrameDataProcess.cpp \
    ../../../src/lcp_sh_d.cpp \
    ../../../src/TcpConnect.cpp \
    ../../../src/TcpAccept.cpp \
    threadtest.cpp


HEADERS  += mainwindow.h \
    ../../../src/Command.h \
    ../../../src/FrameDataProcess.h \
    ../../../src/Handler.h \
    ../../../src/lcp_sh_d.h \
    ../../../src/TcpConnect.h \
    ../../../src/SerHandler.h \
    ../../../src/TcpAccept.h \
    threadtest.h



FORMS    += mainwindow.ui
INCLUDEPATH += ../../../src
INCLUDEPATH += ../../../lib

INCLUDEPATH += $(BOOST_HOME)#boost
LIBS        +=  -lwsock32
LIBS        +=  -lws2_32
LIBS        += -L"$(BOOST_HOME)/lib" -llibboost_system-mgw53-mt-1_59
INCLUDEPATH += E:\CryptoPP\include#cryptopp
LIBS        += -L"E:\CryptoPP\lib" -llibcryptlib

DISTFILES +=


