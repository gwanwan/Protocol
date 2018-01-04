// VsSelution.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"Tcp.h"
#include "toolkit\cast\cast.cc"
namespace cast = qingpei::toolkit::cast;

int main()
{
	string transp = "00131645313231323039313237394E30333130373037363334E3AE";
	vector<uint8_t> tp;
	
	cast::hex_string_to_bytes(transp, tp);
	uint8_t Bit[22] = { 0 };
	int len = tp[2];
	for (size_t i = 0; i < len - 2; i++)
	{
		Bit[i] = tp[i + 3];
	}

	string sql;
	if (Bit[0] != 'F' && Bit[11] != 'F')
	{
		int south, west = 1;
		if (Bit[0] == 'W')west = -1;
		if (Bit[1] == 'S')south = -1;
		uint8_t lg[3] = { 0 };
		lg[0] = Bit[1] ; lg[1] = Bit[2]; lg[2] = Bit[3];
		double longitude = atoi((char*)lg);
		lg[0] = Bit[12] ; lg[1] = Bit[13]; lg[2] = Bit[14] ;
		double latitude = atoi((char*)lg);

		uint8_t la[7] = { 0 };
		uint8_t la_1[7] = { 0 };
		for (size_t i = 0; i < 7; i++)
		{
			la[i] = Bit[i + 4] ;
			la_1[i] = Bit[i + 15];
		}

		double k = atoi((char*)la);
		double k1 = atoi((char*)la_1);
		float tmp = k / (60 * 100000);
		longitude += tmp;
		latitude += k1 / (60 * 100000);

		/*string sql = "update device,register_device set device.longitude = " + std::to_string(longitude*south);
		sql += ",device.latitude = " + std::to_string(latitude*west);
		sql += " where device.addr = register_device.Mac AND register_device.DeviceId ='" + line + "'";
		return sql;*/
		return 0;
	}
}



class test :public DisposeHandler
{
public:
	test(){};

	Dispose a;
public:
	int onWriteDone(const uint8_t *frame, int framelength, TcpClient *clt) 
	{
		string temp = cast::array_to_hex_string<uint8_t>(frame,framelength);

		cout << temp << endl;
		return 0;
	}
	//sock 用来和对应客户端进行通信
	virtual int AlreadyRead(uint8_t *read, int AlreadyReadlength, TcpClient *clt)
	{
		if (AlreadyReadlength == 0)
		{
			return 1;
		}
		if (a.ReadFrameAccordinglength(read, AlreadyReadlength))
		{
			string temp = cast::array_to_hex_string<uint8_t>(a.frame.data(), a.frame.size());
			cout << temp << endl;
		}
		else
		{
			clt->Read();
			
			cout << "wait data" << endl;
		}
		return 0;
	}
	
	virtual int onError(const boost::system::error_code& error, TcpClient *clt) 
	{
		cout << "close tcpclient"<< endl;
		clt->Close();
		return 0;
	}
};

class SerTest :public SerDisposeHandler
{
public:
	Dispose a;

	int onWriteDone(const uint8_t *frame, int framelength, boost::shared_ptr<tcp::socket> ptr, TcpAccept *clt)
	{
		string temp = cast::array_to_hex_string<uint8_t>(frame, framelength);

		cout << temp << endl;
		return 0;
	}
	//sock 用来和对应客户端进行通信
	virtual int AlreadyRead(uint8_t *read, int AlreadyReadlength, boost::shared_ptr<tcp::socket> ptr, TcpAccept *clt)
	{
		if (AlreadyReadlength == 0)return 0;
		if (a.ReadFrameAccordinglength(read, AlreadyReadlength))
		{
			string temp = cast::array_to_hex_string<uint8_t>(a.frame.data(), a.frame.size());
			cout << temp << endl;
		}
		else
		{
			clt->Read(ptr,0);

			cout << "wait data" << endl;
		}
		return 0;
	}
	int onAccept(TcpAccept *ser ,boost::shared_ptr<tcp::socket> ptr) { return 0; };
	virtual int onError(const boost::system::error_code& error, boost::shared_ptr<tcp::socket> ptr, TcpAccept *clt)
	{
		cout << "close tcpclient" << endl;
		clt->Close();
		return 0;
	}
private:
};
/*int main1()
{
	test ab;
	vector<uint8_t> frame;
	frame.push_back(0xaa);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x34);
	frame.push_back(0x4a);
	frame.push_back(0x4c);
	frame.push_back(0x10);
	frame.push_back(0xf6);
	frame.push_back(0x40);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x07);
	frame.push_back(0x00);
	frame.push_back(0x0a);
	frame.push_back(0xff);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x00);
	frame.push_back(0x1b);
	frame.push_back(0xf6);
	frame.push_back(0x55);
	int i = 1;
	
	cout << "***************"<< i++ <<"*****************" << endl;
	io_service io_service_;
	TcpClient clt("172.16.61.250", 3437, io_service_);
	clt.run();
	if (!clt.isConnect() )
	{
		clt.AddHander(&ab);
		clt.SetDealineTime(5);
		clt.write(frame.data(), frame.size());
		clt.Read();
	}
	clt.run();


	
	//clt.Write(send.Frame());//AA000020222426282A000000110080FF0000000A000800010002000300040E7655
	clt.Close();
	cout << "*******************************" << endl;
	return 0;
}*/

/*int _tmain1(int argc, _TCHAR* argv[])
{
	vector<uint8_t> disv;
	disv.push_back(0x02);
	spro::Display dis,disa;
	uint8_t *dis_temp = dis.CreateDis(disv, 0x11, 0x22);
	disv.clear();
	for (int i = 0; i < dis.length(); ++i)
	{
		disv.push_back(dis_temp[i]);
	}
	disa.AnalyDis(disv);

	spro::Controller_Query_Recv td;
	spro::Controller_Query_Recv_1 td_;
	td_.data.push_back(1);
	td_.ID = 0x80;
	td_.length = 1;
	td.data.push_back(td_);
	td.Servilize();
	uint8_t *controller_ID = new uint8_t[8];
	uint8_t *controller = (uint8_t*) "000020222426282A";

	BrightPlan plan;
	plan.Syear = 2016;
	plan.endyear = 2016;
	plan.hour = 24;
	plan.min = 12;
	plan.sec = 10;
	plan.addr.addr = spro::controlID_to_hex(controller, 2 * spro::kMacWidth);
	plan.bright_mode = 1;
	plan.bright_ = 1;
	N_BrightPlan N_b;
	N_b.N_plan.push_back(plan);
	N_b.N_plan.push_back(plan);
	N_b.N_plan.push_back(plan);//时控计划示例
	

	spro::vector<uint8_t> sendd;

	spro::vector<char> rev;

	spro::Controller_Query test_controller;
	spro::vector<uint16_t> cont;
	cont.push_back(0x0001);
	cont.push_back(0x0002);
	test_controller.Attr = cont;

	const char* test = "?€#U";
	spro::DataProcess send;
	spro::Controller_Query  que;
	que.Attr.push_back(0x0001);
	que.Attr.push_back(0x0002);
	que.Attr.push_back(0x0003);
	que.Attr.push_back(0x0004);//控制器属性查询示例
	que.servilize();
								//调用DataProcess进行帧的构造和发送
	uint64_t adr = spro::controlID_to_hex(controller, 2 * spro::kMacWidth);
	send.Hearthbeat(adr);
	send.Controller_ATTR_Query(spro::controlID_to_hex(controller, 2 * spro::kMacWidth), que);
	//send.Controller_Bright_Plan_Query(controlID_to_hex(controller, 2 * kMacWidth), N_b);
	spro::io_service io_service_;
	spro::TcpConnection clt("127.0.0.1", 1000, io_service_);
	//clt.SetDealineTime(3);
	boost::system::error_code ec;
	clt.Read1(0);
	io_service_.run();
	clt.Write(send.Frame());//AA000020222426282A000000110080FF0000000A000800010002000300040E7655
	
	
	

	return 0;
}*/

