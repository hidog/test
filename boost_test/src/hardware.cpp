#include "hardware.h"
#include <boost/thread.hpp>

#include <iostream>






void get_cpu_core_func()
{
    std::cout << "Physical Core : " << boost::thread::physical_concurrency() << std::endl;
    std::cout << "Logical Core : " << boost::thread::hardware_concurrency() << std::endl;
}
