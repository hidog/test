// bosot_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>

#include "../json.h"

#include <mutex>
#include <boost/thread/thread.hpp>



std::mutex mtx;



void func( int i )
{
    throw i;
}


class A
{
public:
    A()
    {
        printf("constructor\n");
    }


    ~A() //throw ()
    {
        printf("destructor\n");        
    }
};



void test_thread( int i )
{
    printf("i = %d\n", i);

    try
    {
        A a;

        std::lock_guard<std::mutex> gud(mtx);
        func(i);
        //while(1);
    }
    catch( int err )
    {
        printf("catch %d!!", err);
    }

}


int _tmain(int argc, _TCHAR* argv[])
{
	// json test.
	//boost_json_example();

    boost::thread thr[3];

    for( int i = 0; i < 3; i++ )
        thr[i]  =   boost::thread( test_thread, i );

    for( int i = 0; i < 3; i++ )
        thr[i].join();

	system("PAUSE");
	return 0;
}

