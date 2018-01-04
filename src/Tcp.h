#pragma once
#include "Handler.h"
#include <list>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio/io_service.hpp>

#define MAXBUFSIZE 1024
#define CONNECTTIMEOUT 1  //连接的超时时间

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::ip::address;
class TcpClient
{
public:
	explicit TcpClient(const std::string & hostname, unsigned short port,
		boost::asio::io_service &io);
	/*
	可以不用传io  内部已经定义 接口保留位了兼容以前
	*/
	TcpClient(boost::asio::io_service &io):
		io_s(io),
		deadline(io_s),
		connecttime(io_s),
		hostname_(""),
		Csocket(io_s){}
public:
	int flag_deadline;
public:
	/*
	发送数据帧，根据长度来发送
	*/
	int write(uint8_t *tm,int length);
	/*
	读取数据帧 ，读到即返回 通过DisposeHandler调用Dispose 来判断是否读取完整的一帧
	*/
	int Read(size_t bytes_transferred = 0);

	/*
	同步的读写
	*/
	int sync_write(uint8_t *frame,int length);
	int sync_read(vector<uint8_t> &frame);

	/*
	关闭
	*/
	void Close() { determine_connect = 1; Csocket.close(); handlerlist.clear(); deadline.cancel();}

	/*
	添加 删除 DispoeseHandler 
	*/
	int AddHander(DisposeHandler* p) { handlerlist.push_back(p); return 0; }
	int RemoveHander(DisposeHandler* p = nullptr);

	/*
	连接函数
	*/
	int Connect();
	void run();
	/*
	超时时间
	*/
	int  SetDealineTime(int Sec);
	void CheckConnectTime();
	void check_deadline(const boost::system::error_code error);
	int isConnect() { return determine_connect; }
	

	/*
	设置hostname
	*/
	int SetHostname(const std::string & hostname, unsigned short port);
private:
	io_service    &io_s;
	tcp::socket   Csocket;
	uint8_t       m_data[MAXBUFSIZE];
	int			  m_length;
	tcp::endpoint endpoint;

	std::string  hostname_;
	unsigned short port_;

	list<DisposeHandler*>  handlerlist;
	DisposeHandler		*  handlerptr;			//由使用者 来进行维护的抽象类指针  在读取和发送完成时会调用对应的onWriteDone 和 onReadDone函数
	
	void handle_connect(const boost::system::error_code &error);
	boost::asio::deadline_timer deadline; //超时时间
	boost::asio::deadline_timer connecttime;
	int           DealineTime;
	
	int			  determine_connect;     //连接标志位 0连接成功
};
/*****************************TcpService**************************************/
class TcpAccept //:public boost::enable_shared_from_this<TcpAccept>
{
public:
	typedef boost::shared_ptr<tcp::socket> sock_pt;
	TcpAccept(boost::asio::io_service &io, tcp::endpoint edp) ://服务端构造函数
		ser_io(io),
		acceptor(ser_io, edp),
		deadline_(io),
		DealineTime(1)
	{
		start();
	}
	TcpAccept(const std::string & hostname, unsigned short port,
		boost::asio::io_service &io);
	void start();
	void accept_handler(const boost::system::error_code& ec, sock_pt sock);
    //void Read(sock_pt ptr);   //读取 指定客户端的内容
	
	

public:
	list<sock_pt>   sock_list;
	list<SerDisposeHandler*>  handlerlist;
	SerDisposeHandler*     handlerptr;			//由使用者 来进行维护的抽象类指针  在读取和发送完成时会调用对应的onWriteDone 和 onReadDone函数
	DataFrameSh     read_frame_;		//读取来的数据解析后一frame结构存在该对象中
	int  AddHander(SerDisposeHandler* p) { handlerlist.push_back(p); return 0; }
	int  RemoveHander(SerDisposeHandler* p = nullptr);
	void Close(sock_pt delete_socket = nullptr);
	
	void readclear();
	void run();

	int  isAccept();//成功0 失败1
					//读取 发送
	
	/*
	发送数据帧，根据长度来发送
	*/
	int write(sock_pt m_socket,uint8_t *tm, int length);
	/*
	读取数据帧 ，读到即返回 通过DisposeHandler调用Dispose 来判断是否读取完整的一帧
	*/
	int Read(sock_pt m_socket, size_t bytes_transferred = 0);

private:

	bool CheckTime();//超时检查

private:
	io_service  & ser_io;
	tcp::socket * temp_socket;
	uint8_t       m_data[MAXBUFSIZE];
	int           have_length; // 记录已读的字节数
	boost::asio::deadline_timer deadline_;
	tcp::acceptor acceptor;
	int           DealineTime;//
	int           accept_temp;//用来对acceptor是否正常关闭进行判断  0表示正常关闭 1表示还开启或出错
	vector<sock_pt> vsockt;
public:
    list<sock_pt>::iterator Rsocklist() { return sock_list.begin(); }//返回sock_list 的头 方便用户操作
	uint16_t addressport() { return temp_socket->remote_endpoint().port(); }

};

