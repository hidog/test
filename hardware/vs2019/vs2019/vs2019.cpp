
#include "../../src/CPU.h"

#include <iostream>

int main()
{
    std::cout << "get core = " << get_real_cpu_core() << std::endl;
    std::cout << "get core = " << get_logical_cpu_core() << std::endl;


    system("PAUSE");
}

