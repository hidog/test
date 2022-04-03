#include "test.h"
#include <iostream>
using namespace std;



Test::Test()
{
    cout << "Test() constructor." << endl;
}





Test::~Test()
{}




int Test::sum( int a, int b )
{
    int i, sum = 0;
    
    for( i = a; i <= b; i++ )
    {
        sum += i;
    }    

    return sum;
}