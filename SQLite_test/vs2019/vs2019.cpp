#include <iostream>
#include "sqlite_test.h"
#include <iostream>

using namespace std;



int main()
{
#if 1
    //sqlite_test_1::create_data();
    sqlite_test_1::create_data_bind();
    cout << endl;

    sqlite_test_1::delete_data();
    cout << endl;

    sqlite_test_1::count_data();
    cout << endl;

    sqlite_test_1::select_data();
    cout << endl;
#endif

#if 0
    sqlite_test_2::create_data();
    cout << endl;

    sqlite_test_2::select_data();
    cout << endl;
#endif

    return 0;
}

