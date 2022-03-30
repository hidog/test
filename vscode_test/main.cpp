#include <iostream>

#include "test.h"

using namespace std;


int main()
{
    cout << "test" << endl;

    Test t;
    cout << t.sum( 123, 562 ) << endl;

    return 0;
}