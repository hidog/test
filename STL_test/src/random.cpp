#include "random.h"
#include <random>
#include <iostream>




void test_c11_rand_1()
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);

    for( int i = 0; i < 10; i++ )
    {
        int r = dist(rd);
        std::cout << r << " ";
    }
    std::cout << std::endl;
}



void test_c11_rand_2()
{
    std::random_device rd;
    std::uniform_real_distribution<double> dist( -10.0, 10.0 );

    for( int i = 0; i < 10; i++ )
    {
        double r = dist(rd);
        std::cout << r << " ";
    }
    std::cout << std::endl;
}



void test_c11_rand_3()
{
    std::random_device rd;
    std::normal_distribution<double> dist( 329.11, 50.3 );

    for( int i = 0; i < 10; i++ )
    {
        double r = dist(rd);
        std::cout << r << " ";
    }
    std::cout << std::endl;
}




void random_func()
{
    test_c11_rand_1();
    test_c11_rand_2();
    test_c11_rand_3();
}
