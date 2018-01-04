
#include"toolkit\cast\cast.h"
#include"FrameDataProcess.h"
#include <boost/iostreams/stream.hpp>
#include"toolkit\cast\cast.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>


using namespace boost;
using namespace boost::asio;
using namespace std;
using namespace sansi::protocol;

boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1000);
std::string data = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!";



class server
{
private:
	io_service &ios;
	ip::tcp::acceptor acceptor;
	typedef boost::shared_ptr<ip::tcp::socket> sock_pt; //智能指针 指向socket对象，用来在回调函数中传递
	DataFrameSh frame_;
	boost::array<string, 1024> msg;
	boost::array<string, 1> recvmsg;
	uint8_t recv[1024];
public:
	//用这个形参初始化成员变量ios， 使用ios,tcp协议和端口号初始化acceptor对象，并用start()函数立即启动异步服务。
	server(io_service& io, string fdata) : ios(io), acceptor(ios, endpoint) {
		for (int i = 0; i < fdata.size(); ++i)msg[i] = fdata[i];
		recvmsg[0] = fdata;
		//msg[0] = fdata;
		start();
	}

	server(io_service& io) : ios(io), acceptor(ios, endpoint) { start(); }
	void start()
	{
		sock_pt sock(new ip::tcp::socket(ios));			//智能指针
		acceptor.async_accept(*sock, bind(&server::accept_handler, this,
			boost::asio::placeholders::error, sock));	//异步侦听服务 //多一个this叫做成员指针
		
	}

	void accept_handler(const boost::system::error_code& ec, sock_pt sock)
	{
		if (ec)
		{
			cout << "accept_handler error." << boost::system::system_error(ec).what() << endl;
			return;
		}

		boost::shared_ptr< vector<char> > str(new vector<char>(100, 0));
		//sock->async_write_some(buffer("hello asio",12), bind(&server::write_handler, this, boost::asio::placeholders::error, sock));
		cout << "client address:" << sock->remote_endpoint().address() << endl;
	    
		sock->async_send(buffer("connection ..."), boost::bind(&server::read_handler, this,
			boost::asio::placeholders::error,sock));

		start();
	}

	void write_handler(const system::error_code& ec, sock_pt sock)
	{
		if (ec)
		{
			cout << "write_handler" << boost::system::system_error(ec).what() << endl;
			return;
		} 
		boost::shared_ptr<vector<char>> str;
		vector<uint8_t> send;
		sock->async_write_some(buffer(send), bind(&server::read_handler, this,
			boost::asio::placeholders::error, sock));
	}
	
	void  read_handler(const system::error_code& ec, sock_pt sock)
	{
		if (ec)
		{
			cout << "read_handler" << boost::system::system_error(ec).what() << endl;
			sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, const_cast<system::error_code&>(ec));
			sock->close(const_cast<system::error_code&>(ec));
			return;
		}
		sock->async_read_some(buffer(recv), bind(&server::recv_handler, this,
			boost::asio::placeholders::error, sock));/* [this, bytessize, sock]
			(boost::system::error_code ec, std::size_t length, sock_pt sock)
		{
			int i = 0;
			while (recv[i] != 0x55 && i < 1024)
			{
				++i;
			}
			if (recv[i] == 0x55)
				cout << cast::array_to_hex_string(recv, ++i);
		}*/
			
			
	}

	void send_handler(const system::error_code& ec, sock_pt sock)
	{
		if (ec)
		{
			cout << "send_handler" << boost::system::system_error(ec).what() << endl;
			return;
		}
		cout << "send msg complete." << endl;

		sock->async_send(buffer(msg), bind(&server::send_handler, this,
			boost::asio::placeholders::error, sock));
		//deadline_timer t(ios, posix_time::seconds(10));
		//t.wait();
	}

	void recv_handler(const system::error_code& ec,sock_pt sock)
	{
		if (ec)
		{
			cout << "recv_handler" << boost::system::system_error(ec).what() << endl;
			return;
		}
		/*sock->async_receive(buffer(msg), bind(&server::recv_handler, this,
			boost::asio::placeholders::error, sock));*/
		int i = 0; int j = 0;
		while (recv[i] != 0xaa && i < 1024)++i;
		j = i;
		//while (i++<kHeaderWidth+j);
		int length=frame_.CheckHead(recv+i);
		if (!length)return;
		
		int framelength = length + kHeaderWidth + kCrcAndEndWidth;
		if (framelength >= 23 && recv[framelength-1+i] == 0x55)
		{
			cout << cast::array_to_hex_string(recv, framelength) << endl;
			frame_.Checkbody(recv+i);
			cout << hex << " command :" << frame_.command() << endl;
			cout << " length :" <<dec <<frame_.FrameLength() << endl;
		}
		std::size_t bytes_transferred;
		const uint8_t *framestr = (frame_).FrameData();
		sock->async_write_some(boost::asio::buffer(framestr, (frame_).FrameLength()),
			[this, bytes_transferred,sock](boost::system::error_code ec, std::size_t length)
		{
			read_handler(ec,sock);
		});
	/*	deadline_timer t(ios, posix_time::seconds(100));
		t.wait();*/

	}
};