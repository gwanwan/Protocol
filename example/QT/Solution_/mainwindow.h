#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include"Handler.h"
#include"TcpAccept.h"
#include"TcpConnect.h"
#include"lcp_sh_d.h"
#include"FrameDataProcess.h"
#include"list"
#include"threadtest.h"
//#include"TcpConnect.h"
namespace Ui {
class MainWindow;
}
using namespace sansi::protocol;


//class DataFrameSh;

class MainWindow : public QMainWindow,public Handler,public SerHandler
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    virtual int onWriteDone(DataFrameSh *write,TcpConnection *clt) ;

    virtual int onReadDone(DataFrameSh *read,TcpConnection *clt);

    virtual int onError(const boost::system::error_code& error) ;
public://ser
    virtual int onWriteDone(sansi::protocol::DataFrameSh *write, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock);
    virtual int onReadDone(sansi::protocol::DataFrameSh *read, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock) ;
    virtual int onError(const boost::system::error_code& error, boost::shared_ptr<tcp::socket> sock);

    virtual int onAccept(sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock){return 0;}
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    TcpConnection *tcp;
    TcpAccept *TcpAcpt;
    Producer *pro;
    sansi::protocol::DataProcess send1;
    //list<TcpConnection> ltcp;
    //io_service io_s;

};


#endif // MAINWINDOW_H
