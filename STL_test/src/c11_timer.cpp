#include "c11_timer.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>




// 二項式定理
int c_n_m( int n, int k )
{
    if( k == 0 )
        return 1;
    else if( n == 0 )
        return 0;
    else
        return c_n_m(n-1, k-1) + c_n_m(n-1, k);
}






void test_c11_timer_1()
{
    // also have system_clock.
    std::chrono::steady_clock::time_point   start   =   std::chrono::steady_clock::now();

    std::cout << "c_n_m(n,m) = " << c_n_m(20,10) << std::endl;

    std::chrono::steady_clock::time_point   end     =   std::chrono::steady_clock::now();

    std::chrono::duration<int, std::milli> du = std::chrono::duration_cast<std::chrono::milliseconds>( end - start );

    std::cout << "duration = " << du.count() << " ms" << std::endl;

    std::cout << "duration = " << std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count() << " ms" << std::endl;

    std::cout << "start time = " << start.time_since_epoch().count() << std::endl;

    std::cout << "epoch: " << start.time_since_epoch().count() << std::endl;
    
    // 沒有to_time_t可以用
}




void test_c11_timer_2()
{
    std::chrono::time_point<std::chrono::system_clock>   start   =   std::chrono::system_clock::now();

    std::cout << "c_n_m(n,m) = " << c_n_m(20,10) << std::endl;

    std::chrono::time_point<std::chrono::system_clock>   end     =   std::chrono::system_clock::now();

    std::cout << "duration = " << std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count() << " ms" << std::endl;

    const std::time_t t_c = std::chrono::system_clock::to_time_t( start );
    std::cout << "time = " << std::put_time(std::localtime(&t_c), "%F %T.\n") << std::endl;
}




void test_c11_timer_3()
{
    using namespace std::chrono;
    typedef duration<int,std::ratio<60*60*24>> days_type;

    time_point<system_clock,days_type> today = time_point_cast<days_type>(system_clock::now());

    std::cout << today.time_since_epoch().count() << " days since epoch" << std::endl;
}




void c11_timer_func()
{
    test_c11_timer_1();
    test_c11_timer_2();
    test_c11_timer_3();
}
