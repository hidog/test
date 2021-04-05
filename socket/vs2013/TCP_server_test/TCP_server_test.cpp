// TCP_server_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../src/TCP_server.h"
#include <cstdlib>

int _tmain(int argc, _TCHAR* argv[])
{
	//tcp_server();
	//tcp_server_multi_thread();
	tcp_server_non_blocking();

	system("PAUSE");
	return 0;
}

