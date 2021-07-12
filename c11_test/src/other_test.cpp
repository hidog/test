#include "other_test.h"

#include <iostream>
#include <string>

using namespace std;


void other_test_1()
{
    ostream_iterator<string> oo{cout};
    *oo = "test 1\n";
    ++oo;
    *oo = "test 2\n";
    
    // this cause compile error
    //++oo;
    //*oo = 123;
}
