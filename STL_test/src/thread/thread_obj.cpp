#include "thread_obj.h"

#include <string>
#include <iostream>
#include <thread>

using namespace std;



void ThreadObj::start( int a, double& b, std::string str )
{
    lock_guard<mutex> g(locker);

    b = 1025.2514 * a;
    cout << "str " << a << " = " << str << endl;
}




void ThreadObj::thr_func_1()
{
    for( int i = 0; i < 10; i++ )
    {
        int d = get();
        this_thread::sleep_for( 100ms );
        if( d < 1000 )
            d = 2*d + i;
        else if( d < 0 )
            d = 5 + i;
        else
            d = 10 + i;
        set(d);

        //cout << data << endl;
    }
}




void ThreadObj::thr_func_2( int index )
{
    for( int i = 0; i < 100; i++ )
    {
        int d = get();
        this_thread::sleep_for( 10ms );
        d = 3*d + 10 - i + index*index;
        if( d > 100000 )
            d = i + index;
        else if ( d < 0 )
            d = 7 + index + i;
        set(d);

        //cout << data << endl;
    }
}



void ThreadObj::thr_func_3( int &index )
{
    for( int i = 0; i < 25; i++ )
    {
        int d = get();
        this_thread::sleep_for( 40ms );
        d =  i + d + 7 + index*3;
        if( d > 10000 )
            d = index;
        else if( d < 0 )
            d = index * index;
        set(d);

        //cout << data << endl;
    }

    index++;
}




void ThreadObj::set( int& a )
{
    locker.lock();    
    data = a;
    locker.unlock();
}



int  ThreadObj::get()
{
    locker.lock();    
    int tmp = data;
    locker.unlock();
    return tmp;
}




void ThreadObj::thread_test_2()
{
    /*
         做實驗
         mutex lock可以用在不同scope的函數
    */
    data = 0;

    thread thr1 = thread{ [this]() {
        for( int i = 0; i < 10; i++ )
            this->set_add_3();
    } };

    thread thr2 = thread{ [this]() {
        for( int i = 0; i < 10; i++ )
        {
            this->set_sub_3();
            this_thread::sleep_for(1ms);
        }
    } };

    thr1.join();
    thr2.join();

    cout << "finish. data = " << data << endl << endl;
}



void ThreadObj::set_add_3()
{
    locker.lock();

    for( int i = 0; i < 10; i++ )
    {
        cout << "ADD   " << i << endl;
        this_thread::sleep_for(50ms);
    }

    data += 3;

    locker.unlock();

    this_thread::sleep_for(1ms);
}




void ThreadObj::set_sub_3()
{
    lock_guard<mutex> lck(locker);
    for( int i = 0; i < 10; i++ )
    {
        cout << " SUB  " << i << endl;
        this_thread::sleep_for(50ms);
    }
    data -= 3;
}


void ThreadObj::thread_test_1()
{
    static int s_count = 0;
    data = 0;

    thr1 = make_unique<thread>( &ThreadObj::thr_func_1, this );
    thr2 = make_unique<thread>( &ThreadObj::thr_func_2, this, s_count );
    thr3 = make_unique<thread>( &ThreadObj::thr_func_3, this, ref(s_count) );

    thr1->join();
    thr2->join();
    thr3->join();

    thr1.release();
    thr2.release();
    thr3.release();

    cout << "data = " << data << endl;

    s_count++;
}
