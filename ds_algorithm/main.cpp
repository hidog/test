// main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "src\linklist.h"

#include <iostream>
#include <stdlib.h>

using namespace std;



int _tmain(int argc, _TCHAR* argv[])
{
	LinkList	list;

	cout << list.size() << endl;
	list.print();

	list.push_back(444);
	list.print();

	list.insert( 0, 3 );
	list.insert( 0, 32 );
	list.insert( 1, 35 );
	for( int i = 0; i < 100; i++ )
		list.insert( i, i + 55 );

	list.print();

	
	cout << "size = " << list.size() << endl;

	LinkList	list2(list);
	list2.print();

	system("PAUSE");
	return 0;
}

