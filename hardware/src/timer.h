#ifndef TIMER_H
#define TIMER_H

// c++11有別的寫法


int c_n_m( int n, int k );



#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif


class Timer
{
public:
	Timer();
	~Timer();

	void start();
	void end();
	void reset();

	void get_time( int& hour, int& min, int& sec, int& msec, int& totalms) const;
	double get_time();

private:
#ifdef _WIN32
	static const LARGE_INTEGER ZERO;

	LARGE_INTEGER t_start;
	LARGE_INTEGER freq;

	LONGLONG total;
#else
   timeval t_start, t_end;
   unsigned long diff;
#endif
};



void test_timer_func();



#endif
