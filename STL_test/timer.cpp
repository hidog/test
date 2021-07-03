#include "timer.h"




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
	totalms = total * 1000 / freq.QuadPart;
#else
    totalms = diff / 1000;
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





