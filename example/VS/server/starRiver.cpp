// starRiver.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ser.h"

int _tmain(int argc, _TCHAR* argv[])
{
	string fame;
	try
	{
		cout << "server start." << endl;
		io_service ios;

		server serv(ios, fame);

		boost::thread_group tg;
		for (int i = 0; i < 3; i++)
			tg.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &ios)));
		tg.join_all();


	}
	catch (std::exception& e)
	{
		cout << e.what() << endl;
	}

	return 0;
}

