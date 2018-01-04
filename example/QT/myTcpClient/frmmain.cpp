#include "frmmain.h"
#include "ui_frmmain.h"
#include "myhelper.h"


string aeskey = "0123456789ABCDEF0123456789ABCDEF";
string keyiv = "ABCDEF0123456789";

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);
    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
}

frmMain::frmMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::frmMain)
{
    ui->setupUi(this);
    testTimer = new QTimer(this);//将定时器超时信号与槽(功能函数)联系起来
    connect( testTimer, SIGNAL(timeout()), this, SLOT(on_stoptranspButton_clicked()) );//开始运行定时器，定时时间间隔为10000ms
    ui->SetNetworkButton->setEnabled(false);

    ui->pushButton_3->setEnabled(false);

    ui->sendButton->setEnabled(false);
    FormHelper::FormInCenter(this);
    FormHelper::FormNotResize(this);    
    QStringList mylist={"11","12","13","14","15","16","17","18","19","20","21","22","23","24","25","26"};
    ui->comboBox->addItems(mylist);
    QRegExp reg1("[0-9a-fA-F]{0,16}");
    ui->txtIP_2->setValidator(new QRegExpValidator(reg1,this));
    QRegExp rx("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator v(rx, 0);
    QLineEdit le;
    le.setValidator(&v);
    le.setInputMask("000.000.000.000");
    ui->txtIP->setValidator(new QRegExpValidator(rx,this));
    QRegExp reg("[0-9a-fA-F]{1,3}[0-9a-dA-d]{0,1}");
    ui->SepanidEdit->setValidator(new QRegExpValidator(reg,this));
    QRegExp portreg("[0-9]{0,7}");
    ui->txtPort->setValidator(new QRegExpValidator(portreg,this));

    QSettings settings("Option.ini", QSettings::IniFormat);
    ui->txtIP->setText(settings.value("DevOption/ip").toByteArray());
    ui->txtIP_2->setText(settings.value("DevOption/control_id").toByteArray());
    ui->txtPort->setText(settings.value("DevOption/port").toByteArray());
    this->Init();
}

frmMain::~frmMain()
{
    delete ui;
}

void frmMain::Init()
{
    ui->btnSend->setEnabled(false);
    ui->txtReceive->setReadOnly(true);
    tcpClient=new QTcpSocket(this);
    tcpClient->abort();//取消原有连接
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ReadData()));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));
}
//                   length    op  st datalen         data             ident end
//aa0000000000000000 00000010 00fe ff 00000009 01 02 39 dfff16 e811 03 0154 55
//aa3132333435363738 0000000f 00fe ff 00000008 01 02 06 aa55 ffee 03   fff0 55   复位
//aa3132333435363738 00000009 00fe ff 00000002 00 00                   ffb5 55  透传开始
//aa3132333435363738 00000009 00fe ff 00000002 02 00                   ffb5 55  透传结束
//aa3132333435363738 00000012 00fe ff 0000000b 01 02 39 1be8 1be7 0b ffc0 03 ffc1 55设置网络参数
//AACCCCCCCCCCCCCCCC 00000010 00FE FF 00000009 01 02 06 AA55 00 FF81 03 FFC1 55
//AA                 0000000F 00FE FF 00000008 01 02 06 AA55    C103 03 C110 55
void frmMain::ReadData()
{
    QByteArray buffer=tcpClient->readAll();
    qDebug("接收数据:"+buffer.toHex());
    if (!buffer.isEmpty())
    {
        if (ui->ckHexReceive->isChecked())
        {
            ui->txtReceive->append("接收数据:"+myHelper::ByteArrayToHexStr(buffer));
        }
        else
        {
            ui->txtReceive->append("接收数据:"+buffer.toHex());
            QByteArray recvdata =buffer;
            string realdata;
            for(int i=0;i<recvdata.size();++i)
                realdata.push_back(recvdata[i]);
            vector<uint8_t> vdata;
            if(Myfame.Analy_Fame(realdata,vdata) == 1)
            {
                QMessageBox::warning(this,tr("提示"),tr("   接收的数据有误   ")
            .arg(ui->txtIP->text()).arg(" !! "));
                return;
            }
            if(Myfame.operation_cout() == 0x00fe)
            {
                if ( testTimer->isActive())
                {
                    testTimer->start(300000);
                }
            }
            if(vdata.size()<7)
            {
                if(vdata.size()==1&&vdata[0] == 0)
                    QMessageBox::warning(this,tr("提示"),tr("   成功进入透传模式   ")
                .arg(ui->txtIP->text()).arg(" !! "));
                else if(vdata[0] == 2)
                    QMessageBox::warning(this,tr("提示"),tr("   退出透传成功   ")
                .arg(ui->txtIP->text()).arg(" !! "));
                return ;
            }
            Coord_debug coordata;
            coordata.Analy(vdata,coordata);//应在Analy_Frame内部
            if(coordata.type == 0x20 && coordata.data[0] == 0x3a)
            {
                recvdata.clear();
                recvdata  = coordata.data.c_str();
                QByteArray intput;
                intput.push_back(coordata.data[2]);
                intput.push_back(coordata.data[1]);
                ui->RepanidEdit->setText(intput.toHex());
                ui->RechannelEdit->setText(QString::number(coordata.data[3]));
                //ui->textEdit->setText(coordata.data.data());
            }
            else if(coordata.type == 0x20)
            {
                if(coordata.data[0]==0x39&&coordata.data[1]==1)
                {
                    QMessageBox::warning(this,tr("提示"),tr("   设置网络参数失败   ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
                else if(coordata.data[0]==0x39&&coordata.data[1]==0)
                {
                    QMessageBox::warning(this,tr("提示"),tr("   设置网络参数成功   ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
                else
                {
                    QMessageBox::warning(this,tr("提示"),tr("   未定义的命令  ！！！   ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
            }
            else if(coordata.type == 0x06)
            {
                if(coordata.data[0]==1)
                {
                    QMessageBox::warning(this,tr("提示"),tr("   复位失败   ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
                else if(coordata.data[0]==0)
                {
                    QMessageBox::warning(this,tr("提示"),tr("   复位成功   ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
                else
                {
                    QMessageBox::warning(this,tr("提示"),tr("  未定义的命令  ")
                   .arg(ui->txtIP->text()).arg(" !! "));
                }
            }
        }
    }
}

void frmMain::ReadError(QAbstractSocket::SocketError)
{
    tcpClient->disconnectFromHost();
    ui->btnConnect->setText("连接");
    ui->txtReceive->append(tr("连接服务器失败,原因:%1").arg(tcpClient->errorString()));
    qDebug("连接服务器失败,原因:%1"+tcpClient->errorString().toLatin1());
}

void frmMain::on_btnConnect_clicked()
{
    if (ui->btnConnect->text()=="连接")
    {
        tcpClient->connectToHost(ui->txtIP->text(),ui->txtPort->text().toInt());
        if (tcpClient->waitForConnected(1000))
        {
            ui->btnConnect->setText("断开");
            ui->txtReceive->append("连接服务器成功");
            ui->btnSend->setEnabled(true);
        }
        QByteArray *array =new QByteArray;
        array->clear();
        array->append(ui->txtIP_2->text());
        Myfame.setcontrolID(array->data());
    }
    else
    {
        tcpClient->disconnectFromHost();
        tcpClient->waitForDisconnected(1000);
        if (tcpClient->state() == QAbstractSocket::UnconnectedState || tcpClient->waitForDisconnected(1000))
        {
            ui->btnConnect->setText("连接");
            ui->txtReceive->append("断开连接成功");
            ui->btnSend->setEnabled(false);
        }
    }
}

void frmMain::on_btnClear_clicked()
{
    ui->txtReceive->clear();
}

void frmMain::on_btnSend_clicked()
{
    QString data=ui->txtSend->text();

    if (data!="")
    {
        if (ui->ckHexSend->isChecked())
        {

            QByteArray *array =new QByteArray;
            array->clear();
            array->append(ui->txtSend->text());
            string test = array->data();
            this->tcpClient->write(test.data(),test.size());
            ui->txtSend->clear();
            //tcpClient->write(myHelper::HexStrToByteArray(data));
        }
        else
        {
            QByteArray *array =new QByteArray;
            array->clear();
            array->append(ui->txtSend->text());
            string test = array->data();
            this->tcpClient->write(test.data(),test.size());
            ui->txtSend->clear();
//            QByteArray *array =new QByteArray;
//            array->clear();
//            array->append(ui->txtSend->text());
//            this->globalString += this->tcpClient->readAll();
//            AES myaes(aeskey,keyiv);
//            myaes.Cipher(array->data());
//            string test = array->data();
//            Myfame.Create_Fame_RE_key_Command(test);
//            this->globalString.clear();test.clear();
//            test=Data_fame::Serialize(Myfame);
//            this->tcpClient->write(test.data());
//            ui->txtSend->clear();
            //tcpClient->write(data.toLatin1());
        }
    }
}

void frmMain::on_SetNetworkButton_clicked()//设置网络参数
{
    Coord_debug rsetcd;string trait;rsetcd.type = 0x20;
    QByteArray *array =new QByteArray;
    array->clear();array->append(ui->SepanidEdit->text());
    if(ui->SepanidEdit->text() == NULL )//|| ui->SechannelEdit->text() == NULL
    {
        QMessageBox::warning(this,tr("提示"),tr("   不能为空   ")
       .arg(ui->txtIP->text()).arg(" !! "));
        return;
    }
    string panid = array->data();
    uint16_t hexdata = Myfame.insertHexnum(panid);
    char *ptr;ptr = (char*)&hexdata;string panidstr;

    for (int i = 0; i < 2; ++i)
        PUSHZERO(ptr,i,panidstr);
    trait.push_back(0x39);
    trait.push_back(panidstr[1]); trait.push_back(panidstr[0]);
    array->clear();
    array->push_back((char)ui->comboBox->currentText().toInt());
    trait.append(array->data());array->clear();

    string rscd = rsetcd.create(trait);
    string test = Myfame.Create_Fame_Send_NetWork(rscd,rscd.size());
    cout<<test<<endl;

    QByteArray *senddata=new QByteArray;
    for(int i =0 ;i<test.size();i++)
         senddata->push_back(test[i]);
    this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
    qDebug("设置网络参数:"+senddata->toHex());
}

void frmMain::on_pushButton_3_clicked()//查询网络参数
{
    Coord_debug rsetcd;string trait;
    trait.push_back(0x3a);
    rsetcd.type = 0x20;
    string rscd = rsetcd.create(trait);

    string test = Myfame.Create_Fame_Search_Network(rscd,rscd.size());

    QByteArray *senddata=new QByteArray;
    for(int i =0 ;i<test.size();i++)
         senddata->push_back(test[i]);
    this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
    qDebug("查询网络参数:"+senddata->toHex());
}

void frmMain::on_sendButton_clicked()//复位
{
    Coord_debug rsetcd;string trait;
    trait.push_back(0xaa);
    trait.push_back(0x55);rsetcd.type = 0x06;
    string rscd = rsetcd.create(trait);
    QByteArray *array =new QByteArray;
    array->clear();
    array->append(rscd.c_str());

    string test = array->data();
    test = Myfame.Create_Fame_Reset_coord(rscd,rscd.size());

    QByteArray *senddata=new QByteArray;
    for(int i =0 ;i<test.size();i++)
         senddata->push_back(test[i]);
    this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
    qDebug("复位:"+senddata->toHex());
}

void frmMain::on_transpButton_clicked()
{
    if (ui->transpButton->text()=="开始透传")
    {
        //停止运行定时器
        testTimer->start(300000);
        ui->SetNetworkButton->setEnabled(true);
        //ui->pushButton->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
      //  ui->sendButton_2->setEnabled(true);
        ui->sendButton->setEnabled(true);
        string test = Myfame.Create_Fame_CONTROL_MSG_SEND(1);//1表示开始透传
        //aa 0015081800000000 0000000e 00fe ee 00000007 02 06 aa55 ffef 03 0a 0014 55
        //aa 00e04f1600000000 00000019 00fe ff 00000002 00 00              31 cf01 55
        QByteArray *senddata=new QByteArray;
        for(int i =0 ;i<test.size();i++)
             senddata->push_back(test[i]);
        this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
        ui->transpButton->setText("停止透传");
    }
    else
    {
        ui->SetNetworkButton->setEnabled(false);
        //ui->pushButton->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        //ui->sendButton_2->setEnabled(false);
        ui->sendButton->setEnabled(false);
        string test = Myfame.Create_Fame_CONTROL_MSG_SEND(0);//0表示停止透传
        QByteArray *senddata=new QByteArray;
        for(int i =0 ;i<test.size();i++)
             senddata->push_back(test[i]);
        this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
        ui->transpButton->setText("开始透传");
        if ( testTimer->isActive())
        {
            testTimer->stop();
        }
    }
}

void frmMain::on_stoptranspButton_clicked()
{
      QMessageBox::about(NULL, "rset time", "透传停止");
      ui->SetNetworkButton->setEnabled(false);
     // ui->pushButton->setEnabled(false);
      ui->pushButton_3->setEnabled(false);
      //ui->sendButton_2->setEnabled(false);
      ui->sendButton->setEnabled(false);
      string test = Myfame.Create_Fame_CONTROL_MSG_SEND(0);//0表示停止透传
      QByteArray *senddata=new QByteArray;
      for(int i =0 ;i<test.size();i++)
           senddata->push_back(test[i]);
      this->tcpClient->write(senddata->toStdString().data(),senddata->toStdString().size());
      ui->transpButton->setText("开始透传");
      if ( testTimer->isActive())
      {
          testTimer->stop();
      }
}

void frmMain::on_toolButton_clicked()
{

    QSettings settings("Option.ini", QSettings::IniFormat); // 当前目录的INI文件
    settings.beginGroup("DevOption");
    settings.setValue("ip", ui->txtIP->text());
    settings.setValue("control_id",ui->txtIP_2->text());
    settings.setValue("port",ui->txtPort->text());
    settings.endGroup();
    //file.close;
}

void frmMain::on_txtIP_2_cursorPositionChanged(int arg1, int arg2)
{

}

void frmMain::on_SepanidEdit_cursorPositionChanged(int arg1, int arg2)
{

}
