#include "timer.h"
#include <iostream>

using namespace std;







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









#ifdef _WIN32
const LARGE_INTEGER Timer::ZERO = { 0 };
#endif





Timer::Timer()
{
	reset();
#ifdef _WIN32
	QueryPerformanceFrequency(&freq);
#endif
}




Timer::~Timer()
{}




void Timer::start()
{
#ifdef _WIN32
	QueryPerformanceCounter(&t_start);
#else
    gettimeofday( &t_start, NULL );
#endif
}




void Timer::end()
{
#ifdef _WIN32
	LARGE_INTEGER freq = { 0 };
	QueryPerformanceCounter(&freq);
	total += freq.QuadPart - t_start.QuadPart;
#else
    gettimeofday( &t_end, NULL );
    diff += 1000000 * (t_end.tv_sec - t_start.tv_sec) + t_end.tv_usec - t_start.tv_usec;
#endif
}




void Timer::reset()
{
#ifdef _WIN32
	total = 0;
	t_start = ZERO;
#else
    diff = 0;
#endif
}




void Timer::get_time( int& hour, int& min, int& sec, int& msec, int& totalms) const
{
#ifdef _WIN32
	totalms = static_cast<int>( total*1000/freq.QuadPart );
#else
    totalms = static_cast<int>(diff / 1000);
#endif
	hour = totalms / 1000 / 60 / 60;
	min = totalms / 1000 / 60 % 60;
	sec = totalms / 1000 % 60;
	msec = totalms % 1000;
}





double Timer::get_time()
{
#ifdef _WIN32
	return static_cast<double>(total) * 1000 / freq.QuadPart;
#else
    return diff / 1000;
#endif
}





void test_timer_func()
{
    Timer timer;
    timer.start();

    int value = c_n_m( 20, 10 );
    cout << "c_n_m = " << value << endl;

    timer.end();

    int hour, min, sec, msec, totalms;
    timer.get_time( hour, min, sec, msec, totalms );
    std::cout << "timer = " << timer.get_time() << "ms\n";
    std::cout << hour << " " << min << " " << sec << " " << msec << " " << totalms  << "\n";
}


