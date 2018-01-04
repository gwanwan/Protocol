#include"threadtest.h"

Producer::Producer(QObject *parent) : QThread(parent)
{

}

int Producer::onWriteDone(sansi::protocol::DataFrameSh *write, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock)
{

    cout<<"write done"<<endl;
    boost::system::error_code error;
    const uint16_t port = sock->remote_endpoint().port();
    TcpAcpt->Read(port);

   // TcpAcpt->Readhand(error,sock);

    return 1;
}


int Producer::onReadDone(sansi::protocol::DataFrameSh *read, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock)
{

    Controller_Query  que;
    que.Attr.push_back(0x0001);
    que.Attr.push_back(0x0002);
    que.Attr.push_back(0x0003);
    que.Attr.push_back(0x0004);//

    que.servilize();
    RSmartEleQuer ELe;
    Controller_Query_Recv_1 control;
    control.state = 0x01;
    control.ID = 0x0011;
    control.length = 1;
    control.data.push_back(1);
    ELe.addr[2] = '5';
    ELe.statu = 0x01;
    ELe.RID.push_back(control);
    ELe.servilize();
    cout<<" ELe.Serdata "<<ELe.Serdata.data() <<endl;
    RSmartEleQuer tempE;
    tempE.Deservilize(ELe.Serdata);
    //sansi::protocol::DataProcess send1;
    send1.CreateFrame(0xAA11223344556701,que.command,que.Serdata);
    TcpAcpt->Write(sock,send1.Frame());
    cout<<"read done"<<endl;
    return 1;
}
int Producer::onError(const boost::system::error_code& error,boost::shared_ptr<tcp::socket> sock )
{
    sock->remote_endpoint().port();

    cout<<"error "<<sock->remote_endpoint().port() <<endl;
    return 1;
}


void Producer::run()
{
    io_service io_service_;
    boost::asio::ip::tcp::endpoint endp(ip::tcp::v4(),1000);
    TcpAcpt = new TcpAccept(io_service_,endp);
    TcpAcpt->AddHander(this);
    if(!TcpAcpt->isAccept())
        cout<<"listen ..."<<endl;
    io_service_.reset();
    TcpAcpt->run();

    //
}
