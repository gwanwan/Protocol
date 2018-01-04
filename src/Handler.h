#ifndef _HANDLER_H_INCLUDE_
#define _HANDLER_H_INCLUDE_

#include"lcp_sh_d.h"
#include<boost/asio.hpp>
#include<list>
using boost::asio::ip::tcp;

class TcpConnection;
class DataFrameSh;
class TcpClient;
class TcpService;
class TcpAccept;// *ser

/*  Hand 类进行数据的处理  
1 进行数据帧是否完整的判断，不做深入处理  
2 将受到的完整数据帧 传给（onReadDone）app 由app处理后返回是否正确来决定接下 动作
3 如果是错误的帧 将缓冲区内从新查找 aa 开头 或对应数据帧特征的开头 将之前的其他数据舍弃
const uint8_t kFrameStart = 0xAA;
const uint8_t kFrameEnd = 0x55;
*/


const uint8_t kFrameStart_ = 0xAA;
const uint8_t kFrameEnd_ = 0x55;
const int MaxBuffSize = 1024;
const int IntLength = 4;
const int Endlength_ = 3;


class Dispose  {
public:
    Dispose(int LengthSite_ = 9, uint8_t Head_ = kFrameStart_, uint8_t End_ = kFrameEnd_, int supplementlength = Endlength_) :
		LengthSite(LengthSite_),
		Head(Head_),
		End(End_),
		supplement(supplementlength)
    {}
	/*
	根据长度读取数据
	1.当tcp收到消息时会将信息传入dispose 在传入的数据中寻找 Head标志位 将缓冲区内head标志位前的数据丢掉
	2.结束标准读到需要长度时 判断尾部是否符合标准 若不符合 在缓冲区中后移一位继续读取或者直接判断 等到读到符合要求的数据
	3.读到完整的一帧 就返回1 否则为0继续读取
	*/
	int ReadFrameAccordinglength(uint8_t *data,int Readlength);
	
	/*
	根据头尾读取数据
	1.当tcp收到消息时会将信息传入dispose 在传入的数据中寻找 Head标志位 将缓冲区内head标志位前的数据丢掉
	2.直至读到 end为止
	*/
	int ReadFrameHeadEnd(uint8_t *data, int Readlength);

private:
	/*
	读取数据时候 查找Head并将不匹配的数据去除
	*/
    int FindHead(uint8_t Head_ = kFrameStart_);
	
	/*
	当数据读取完毕时进行frame 的检查
	*/
    int CheckEnd(uint8_t End_ = kFrameEnd_);

	/*
	根据长度读取数据
	*/
    int PushData(uint8_t *data, int Readlength_);
	/*
	计算数据的长度
	*/
	int FrameLength();

	/*
	获取到完整的一帧
	*/
	int GetFrame(int framelength);
	int GetFrame();
private:
	/*
	缓冲区用来存储读来的数据头为Head
	*/
	std::list<uint8_t> FrameBuff;
public:
	/*
	当数据读取完毕时 放到这当中 传给用户
	*/
	std::vector<uint8_t> frame;	

public:
	int LengthSite;
	uint8_t Head;
	uint8_t End;
	int supplement;//对给定长度也无法完全描述的给予补充长度
};

class DisposeHandler 
{
public:
	virtual ~DisposeHandler() {}
	virtual int onWriteDone(const uint8_t *frame,int framelength, TcpClient *clt) = 0;
	//sock 用来和对应客户端进行通信
	virtual int AlreadyRead(uint8_t *read, int AlreadyReadlength, TcpClient *clt) = 0;
	virtual int onError(const boost::system::error_code& error, TcpClient *clt) = 0;
	
	
	void Destory() { delete this; }
};

//class Handler //:public DataFrameSh
//{
//public:
//	virtual ~Handler(){}
//    virtual int onWriteDone(DataFrameSh *write, TcpConnection *clt) = 0;
//    virtual int onReadDone(DataFrameSh *read, TcpConnection *clt) = 0;
//    virtual int onError(const boost::system::error_code& error) = 0;
//	//static Handler* _stdcall CreateHandle(){ return nullptr; };
//	void Destory(){ delete this; }
//};

class SerDisposeHandler
{
public:
	virtual ~SerDisposeHandler() {}
	virtual int onWriteDone(const uint8_t *frame, int framelength, boost::shared_ptr<tcp::socket> ptr, TcpAccept *ser) = 0;
	//sock 用来和对应客户端进行通信
	virtual int AlreadyRead(uint8_t *read, int AlreadyReadlength, boost::shared_ptr<tcp::socket> ptr, TcpAccept *ser) = 0;
	virtual int onAccept(TcpAccept *ser ,boost::shared_ptr<tcp::socket> ptr)=0;
	virtual int onError(const boost::system::error_code& error, boost::shared_ptr<tcp::socket> ptr, TcpAccept *ser) = 0;


	//static Handler* _stdcall CreateHandle(){ return nullptr; };
	void Destory() { delete this; }
};

#endif

