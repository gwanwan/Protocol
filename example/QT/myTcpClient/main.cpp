#include "frmmain.h"
#include <QApplication>
#include<QtMsgHandler>
#include "myhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    myHelper::SetUTF8Code();
    myHelper::SetPlastiqueStyle();

    frmMain w;
    w.show();



       //注册MessageHandler
       qInstallMessageHandler(outputMessage);

       //打印日志到文件中
       qDebug("This is a debug message");
       qWarning("This is a warning message");
       qCritical("This is a critical message");
       //qFatal("This is a fatal message");
    return a.exec();
}
