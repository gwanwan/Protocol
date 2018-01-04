#ifndef THREADTEST_H
#define THREADTEST_H

#include"TcpAccept.h"
#include"QThread"
#include"FrameDataProcess.h"

using namespace  sansi::protocol;
class Producer:public QThread,public SerHandler
{
    Q_OBJECT
public:
    friend class TcpAccept;
    Producer(QObject *parent = 0);
    virtual int onWriteDone(sansi::protocol::DataFrameSh *write, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock);
    virtual int onReadDone(sansi::protocol::DataFrameSh *read, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock) ;
    virtual int onError(const boost::system::error_code& error,boost::shared_ptr<tcp::socket> sock);
    virtual int onAccept(sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock){return 0;}
    sansi::protocol::DataProcess send1;
public:
    void run();
    TcpAccept *TcpAcpt;
};

#endif // THREADTEST_H
