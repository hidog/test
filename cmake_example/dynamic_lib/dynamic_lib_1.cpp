#include "dynamic_lib_1.h"
#include <iostream>





DynamicObj::DynamicObj()
{}


DynamicObj::~DynamicObj()
{}

void DynamicObj::test_func()
{}




int dynamic_func_1()
{
    std::cout << "test dynamic lib" << std::endl;
    return 10;
}


