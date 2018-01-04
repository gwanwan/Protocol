#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QWidget>
#include <QTcpSocket>
#include<QTime>
#include "protocols.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QByteArray>
#include <QMessageBox>
#include<QDebug>


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);


namespace Ui {
class frmMain;
}

class frmMain : public QWidget
{
    Q_OBJECT
    
public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();
    
private slots:
    void on_btnConnect_clicked();

    void ReadData();
    void ReadError(QAbstractSocket::SocketError);

    void on_btnClear_clicked();

    void on_btnSend_clicked();

    void on_SetNetworkButton_clicked();

    void on_pushButton_3_clicked();

    void on_sendButton_clicked();

    void on_transpButton_clicked();

    void on_stoptranspButton_clicked();

    void on_toolButton_clicked();

    void on_txtIP_2_cursorPositionChanged(int arg1, int arg2);

    void on_SepanidEdit_cursorPositionChanged(int arg1, int arg2);

private:
    Ui::frmMain *ui;
    Data_fame  Myfame;
    QTcpSocket *tcpClient;
    QString globalString;
    QTimer *testTimer;

    void Init();
};

#endif // FRMMAIN_H
