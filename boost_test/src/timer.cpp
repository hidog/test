#include "timer.h"

#include <boost/asio.hpp>

#include <iostream>
#include <chrono>


boost::asio::io_service io;
boost::asio::deadline_timer timer(io);



void handler( boost::system::error_code ec )
{
    static std::chrono::steady_clock::time_point last = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    std::chrono::duration<int, std::milli> du = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);

    std::cout << "duration = " << du.count() << std::endl;

    timer.expires_from_now(boost::posix_time::milliseconds(1000));
    timer.async_wait( handler );
    io.run();

    last = now;
}



void timer_test()
{
    timer.expires_from_now(boost::posix_time::milliseconds(1000));
    timer.async_wait(handler);    
    io.run();
}
