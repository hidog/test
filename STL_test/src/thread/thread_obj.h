#ifndef THREAD_OBJ_H
#define THREAD_OBJ_H

#include <string>
#include <mutex>
#include <memory>
#include <thread>



class ThreadObj
{
public:
    ThreadObj() = default;
    ~ThreadObj() = default;
    ThreadObj( const ThreadObj& ) = delete;
    ThreadObj( ThreadObj&& ) = delete;
    ThreadObj& operator = ( const ThreadObj& ) = delete;
    ThreadObj& operator = ( ThreadObj&& ) = delete;

    void start( int a, double& b, std::string str );

    void thread_test_1();
    void thr_func_1();
    void thr_func_2( int index );
    void thr_func_3( int &index );

    void thread_test_2();
    void set_add_3();
    void set_sub_3();
    

    std::unique_ptr<std::thread> thr1, thr2, thr3;
    
    std::mutex locker;

    int data = 0;
    void set( int& a );
    int  get();
};




#endif