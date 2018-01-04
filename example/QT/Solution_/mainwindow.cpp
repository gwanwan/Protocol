#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"toolkit/cast/cast.h"
#include<QString>
using namespace sansi::protocol;
boost::asio::ip::tcp::endpoint endp(ip::tcp::v4(),1000);

byte aeskey[CryptoPP::AES::DEFAULT_KEYLENGTH] =
{ 0x93, 0x33, 0x6B, 0x82, 0xD6, 0x64, 0xB2, 0x46,
0x95, 0xAB, 0x89, 0x91, 0xD3, 0xE5, 0xDC, 0xB0 };

byte aesiv[CryptoPP::AES::BLOCKSIZE] =
{ 0x61, 0x4D, 0xCA, 0x6F, 0xB2, 0x56, 0xF1, 0xDB,
0x0B, 0x24, 0x5D, 0xCF, 0xB4, 0xBD, 0xB6, 0xD3 };
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pro = new Producer();
    DataProcess temp,anly;
    temp.SetAesKey(aeskey,aesiv);
    Controller_Query  que;
    que.Attr.push_back(0x0001);
    que.Attr.push_back(0x0002);
    que.Attr.push_back(0x0003);
    que.Attr.push_back(0x0004);//
    que.servilize();
    //sansi::protocol::DataProcess send1;
    temp.CreateFrame(0x0000aabbccddeeff,que.command,que.Serdata);
    const uint8_t *te = temp.frame();//<<"  dataframe"<<endl;
    vector<uint8_t> analydata;
    int length =temp.framelength();
    for(int i=0;i<length;++i)
    {
        analydata.push_back(te[i]);
    }
    anly.SetAesKey(aeskey,aesiv);
    anly.AnalyFrame(analydata);
//    AesCryp2str t(aeskey,aesiv);
//    string temp = "asdfghjkl";
//    string en,de;
//    t.AESEncryptStr(temp,en);
//    cout<<en<<"en crytopp"<<endl;
//    t.AESDecryptStr(en,de);
//    cout<<de<<"de crytopp"<<endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}


//*********************ser ***************************
int MainWindow::onWriteDone(sansi::protocol::DataFrameSh *write, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock)
{

    cout<<"write done"<<endl;
    return 1;
}


int MainWindow::onReadDone(sansi::protocol::DataFrameSh *read, sansi::protocol::TcpAccept *ser,boost::shared_ptr<tcp::socket> sock)
{
    uint16_t port = ser->addressport();
    string frame = cast::array_to_hex_string(read->FrameData(),read->FrameLength());;
    QByteArray *array =new QByteArray;
    array->append(ui->lineEdit->text());
    ui->textEdit_2->append(tr("[clientID:%1]接收数据：%2").arg(port).arg(frame.data()));
    qApp->processEvents();
    Controller_Query  que;
    que.Attr.push_back(0x0001);
    que.Attr.push_back(0x0002);
    que.Attr.push_back(0x0003);
    que.Attr.push_back(0x0004);//
    que.servilize();
    //sansi::protocol::DataProcess send1;
    send1.CreateFrame(0x0000aabbccddeeff,que.command,que.Serdata);
    TcpAcpt->Write(sock,send1.Frame());
    cout<<"read done"<<endl;
    return 1;
}
int MainWindow::onError(const boost::system::error_code& error,boost::shared_ptr<tcp::socket> sock)
{
    cout<<"error "<<endl;

    return 1;
}



/*****************************************************/

int MainWindow::onWriteDone(DataFrameSh *write,TcpConnection *clt_)
{
    cout<< write->command()<<endl;
    clt_->Read();
    cout<<"connect :"<<clt_->isConnect()<<endl;
    return 1;
}
int MainWindow::onReadDone(DataFrameSh *read,TcpConnection *clt_)
{   //
    string showaddr = cast::num_to_hex_string(read->Addr());
    string frame =  cast::array_to_hex_string(read->FrameData(),read->FrameLength());
    ui->textEdit->setText(showaddr.data());
    ui->textEdit_2->append(frame.data());
    qApp->processEvents();//refresh
    vector<uint8_t>  temp;
    read->Data(temp);
    cout<<endl;
    sansi::protocol::Controller_Query que;
    sansi::protocol::N_BrightPlan N_b;
    TerminalCritQuery tque;
    RTerminalCritQuery rque;
   // tque.Deservilize(temp);
    rque.Deservilize(temp);
    Controller_Query_Recv_1 *ctemp;
    ctemp = &rque.data[0];
    cout<<"RTerminalCritQuery length:"<<rque.data_length<<endl;
    cout<<"RTerminalCritQuery ad:"<<rque.addr<<endl;
    cout<<"RTerminalCritQuery ID:"<<ctemp->ID<<endl;
//    BrightPlan  *k = &N_b.N_plan[0];

    int l=1;
  //  cout<<(k->bright_mode == l)<<endl;
    uint16_t i=1;
    cout<<i<<endl;


}
int MainWindow::onError(const boost::system::error_code& error)
{

    cout<<"ONerror "<<error.message()<<endl;
    tcp->Close();
    return 1;
}
void MainWindow::on_pushButton_clicked()
{
    QByteArray *array =new QByteArray;
    array->append(ui->lineEdit->text());
    uint64_t ad = controlID_to_hex((uint8_t*)array->toStdString().data(),2*kMacWidth);// controller address
    TerminalCritQuery tque;
    Controller_Query_Recv_1 temp;
    temp.ID = 0x0002;
    temp.state =0x01;
    temp.length = 0x0001;
    temp.data.push_back(1);
    RTerminalCritQuery rque;//??????????????
    rque.data.push_back(temp);
    //AA7744ABABABABABAB 00000017 1080 FF 00000010 C5 10 7744ABABABABABAB 0002 01 0001 01 1AC1 55

    rque.addr = ad;
    rque.Servilize();

    Controller_Query  que;
    que.Attr.push_back(0x0001);
    que.Attr.push_back(0x0002);
    que.Attr.push_back(0x0003);
    que.Attr.push_back(0x0004);//???????????????
    //send.Controller_ATTR_Query(controlID_to_hex(controller, 2 * kMacWidth), que);
    Controller_Query_Recv_1 recv1;
    recv1.data.push_back(3);
    recv1.ID=2;
    recv1.state = 0;
    recv1.length =1;
    Controller_Query_Recv recv;//??????????????? ser
    recv.data.push_back(recv1);
    recv.data.push_back(recv1);

    sansi::protocol::DataProcess send;
    sansi::protocol::DataProcess send1;

    send.Hearthbeat(ad);
    recv.Servilize();
    send1.CreateFrame(ad,rque.command,rque.Serdata);
    sansi::protocol::BrightPlan plan;
    plan.Syear = 2016;
    plan.endyear = 2016;
    plan.hour = 24;
    plan.min = 12;
    plan.sec = 10;
    plan.addr.addr = ad;
    plan.bright_mode = 1;
    plan.bright_ = 1;
    sansi::protocol::N_BrightPlan N_b;

    N_b.N_plan.push_back(plan);//????????
    int i=0;io_service io_service_;
    tcp = new TcpConnection("127.0.0.1", 1000,io_service_);
    tcp->AddHander(this);
    tcp->run();
    //io_service_.reset(); reset IO run can reboot
    tcp->SetDealineTime(2);//  settimeout under connect check

    if(!tcp->isConnect())
    {
        tcp->Write(send1.Frame());
    }
    tcp->run();

    //AA000020222426282A000000110080FF0000000A000800010002000300040E7655
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->textEdit_2->clear();
}

void MainWindow::on_pushButton_3_clicked()
{
//    io_service io_service_;
//    boost::asio::ip::tcp::endpoint endp(ip::tcp::v4(),1000);
//    TcpAcpt = new TcpAccept(io_service_,endp);
//    TcpAcpt->AddHander(this);
//    if(!TcpAcpt->isAccept())
//        cout<<"listen ..."<<endl;
//    TcpAcpt->run();
//      pro->run();
//      uint64_t temp = 12254613664636954369;
//      uint64_t temp1= 0xAA11223344556701;
//      if(temp ==  temp1 )
//        cout<<"12254613664636954369"<<hex<<temp<<endl;
//    Producer p;
      pro->start();
//    p.wait();
}



void MainWindow::on_pushButton_4_clicked()
{
    pro->TcpAcpt->Close();
    pro->quit();
}
