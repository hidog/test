// TCP_client_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../src/TCP_client.h"
#include <cstdlib>
#include <thread>

int _tmain(int argc, _TCHAR* argv[])
{
	//tcp_client();

	int i;
	std::thread *thr[8];
	for( i = 0; i < 8; i++ )	
		thr[i] = new std::thread(tcp_client_multi_thread);
	
	for( i = 0; i < 8; i++ )	
		thr[i]->join();

	for( i = 0; i < 8; i++ )
		delete thr[i];


	system("PAUSE");
	return 0;
}

