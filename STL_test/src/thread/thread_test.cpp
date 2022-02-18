#include "thread_test.h"

#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include <stdio.h>
#include <vector>
#include <list>
#include <memory>
#include <algorithm>


#include "thread_obj.h"


using namespace std;


std::mutex mtx_basic_1;
std::mutex mtx_basic_2;

int g_count = 0;
//std::atomic_int g_count = 0;

constexpr int LOOP_COUNT = 10;
constexpr int THREAD_COUNT = 5;

unique_ptr<int> p_count { new int{0} };




void thr_basic_func_1()
{
    for( int i = 0; i < LOOP_COUNT; i++ )
    {
        mtx_basic_1.lock();
        g_count++;
        mtx_basic_1.unlock();
    }
}



void thread_basic_test_1()
{
    int i;
    g_count = 0;
    std::thread thr[THREAD_COUNT];

    for( i = 0; i < THREAD_COUNT; i++ )    
        thr[i] = std::thread( thr_basic_func_1 );

    for( i = 0; i < THREAD_COUNT; i++ )
        cout << "thr[" << i << "] id = " << thr[i].get_id() << endl;
    
    for( i = 0; i < THREAD_COUNT; i++ )
        thr[i].join();

    cout << endl << "g_count = " << g_count << ", and expect value = " << LOOP_COUNT * THREAD_COUNT << endl << endl;
}







void thr_basic_func_2( int index )
{
    // 加上這行後, 會讓每個 loop 循序執行. (驗證lock效果,但實際上不會這樣寫)
    std::lock_guard<std::mutex> guard(mtx_basic_2); 

    for( g_count = 0; g_count < 100; g_count++ )
    {
        printf("the %d round\n", 10000*index + g_count );
    }
}





void thread_basic_test_2()
{
    int i;
    g_count = 0;
    std::thread thr[THREAD_COUNT];

    for( i = 0; i < THREAD_COUNT; i++ )    
        thr[i] = std::thread( thr_basic_func_2, i );

    for( i = 0; i < THREAD_COUNT; i++ )
        thr[i].join();

    cout << endl;
}





void thr_basic_func_3( std::string& str, int index )
{
    str = std::string("This is thr_basic_func_3 test. index = ") + std::to_string(index);
}



void thread_basic_test_3()
{
    thread *thr1, *thr2;
    std::string str1, str2;

    thr1 = new thread( thr_basic_func_3, std::ref(str1), 1 );   // reference需要加上ref
    thr2 = new thread( thr_basic_func_3, std::ref(str2), 2 );

    thr1->join();
    thr2->join();

    cout << "str1 = " << str1 << "\nstr2 = " << str2 << endl << endl;

    delete thr1;
    delete thr2;
}




void thread_sleep_test()
{
    cout << "sleep for a while\n";
    std::this_thread::sleep_for(1000ms);
    cout << "wake up.\n\n";

    cout << "sleep for a while, part 2\n";
    std::this_thread::sleep_for( chrono::milliseconds(1000) );
    cout << "wake up, part 2\n\n";
}




void thread_c11_test_1()
{
    thread thr[THREAD_COUNT];
    g_count = 0;

    for( int i = 0; i < THREAD_COUNT; i++ )
    {
        thread tmp_thr{ thr_basic_func_2, i };
        thr[i] = std::move(tmp_thr);
    }

    for( auto &t : thr )
        t.join();

    cout << "finish" << endl << endl;
}




void thread_c11_test_2()
{
    list<thread> thr_list;
    //vector<thread> thr_list;


    for( int i = 0; i < THREAD_COUNT; i++ )
        thr_list.emplace_back( thr_basic_func_2, i );

    for( auto &t : thr_list )
        t.join();

    cout << "finish" << endl << endl;
}






void thr_c11_test_3( std::shared_ptr<int> p )
{
    for( int i = 0; i < LOOP_COUNT; i++ )
    {
        (*p_count)++;
        (*p)++;
    }
}





void thread_c11_test_3()
{
    int i;

    unique_ptr<thread[]> thr{ new thread[THREAD_COUNT] };
    *p_count = 0;
    auto p = make_shared<int>(0);

    for( int i = 0; i < THREAD_COUNT; i++ )    
        thr[i] = thread{ thr_c11_test_3, p };
    
    for( i = 0; i < THREAD_COUNT; i++ )
        thr[i].join();

    cout << "*p = " << *p << endl;
    cout << "*p_count = " << *p_count << ", expect value = " << THREAD_COUNT*LOOP_COUNT << endl << endl;
}





void thread_obj_test_1()
{
    int i;

    ThreadObj obj;
    vector<thread> thr(THREAD_COUNT);
    vector<double> data(THREAD_COUNT);
    std::string str;

#if 1
    i = 0;
    for_each( thr.begin(), thr.end(), [&]( auto &t ) {
            i++;
            str += to_string(i);
            t = thread( &ThreadObj::start, &obj, i, std::ref(data[i-1]), str );
        } );
#else
    for( i = 0; i < THREAD_COUNT; i++ )
    {
        str += to_string(i);
        thr[i] = thread( &ThreadObj::start, &obj, i, std::ref(data[i]), str );
    }
#endif


    for( i = 0; i < THREAD_COUNT; i++ )
        thr[i].join();

    for( i = 0; i < THREAD_COUNT; i++ )
        cout << "data[i] = " << data[i] << endl;
    cout << endl;
}





void thread_c11_test_4()
{
    string str;
    
    thread thr{ []( std::string& str ) {
        for( int i = 0; i < LOOP_COUNT; i++ )
            str += to_string(i);
    }, ref(str) };

    thr.join();

    cout << "str = " << str << endl << endl;
}




void thread_obj_test_2()
{
    ThreadObj obj;

    for( int i = 0; i < 5; i++ )
        obj.thread_test_1();

    cout << endl;
}






void thread_obj_test_3()
{
    ThreadObj obj;
    obj.thread_test_2();
}







void thread_entry_func()
{
    cout << "core = " << std::thread::hardware_concurrency() << endl;

    thread_basic_test_1();
    thread_basic_test_2();
    thread_basic_test_3();

    thread_sleep_test();

    thread_c11_test_1();
    thread_c11_test_2();
    thread_c11_test_3();
    thread_c11_test_4();

    thread_obj_test_1();
    thread_obj_test_2();
    thread_obj_test_3();
}