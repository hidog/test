#include <iostream>
#include <cstdlib>
#include "c11_sample.h"
#include "c11_initial.h"
#include "c11_for_each.h"

int main()
{
    std::cout << "This is test.\n";

    c11::uniform_initial_test();
    c11::for_each_example();

    c11_sample_3();
    c11_sample_4();
    c11_sample_5();
    c11_sample_6();
    c11_sample_7();
    c11_sample_8();
    //c11_sample_9();
    c11_sample_10();
    c11_sample_11();
    c11_sample_12();
    c11_sample_13();
    c11_swap();

    system("PAUSE");
}

