#include "dynamic_lib_1.h"
#include <iostream>

using namespace std;



DynamicObj::DynamicObj()
{
    cout << "DynamicObj constructor" << endl;
}



DynamicObj::~DynamicObj()
{
    cout << "DynamicObj destructor" << endl;
}




void DynamicObj::test_func()
{
    cout << "file = " << __FILE__ << "\nfunction = " << __FUNCTION__ << "\nline = " << __LINE__ << endl;
}




int dynamic_func_1()
{
    printf("dynamic func test.\n");

#ifdef __cplusplus
    int cpp_def = __cplusplus;
#else
    int cpp_def = 0;
#endif

#ifdef __STDC__
    int c_def = __STDC__;
#else
    int c_def = 0;
#endif

    printf("cpp_def = %d, c_def = %d\n", cpp_def, c_def );
    return 10;
}




int dynamic_func_4()
{
    cout << "This function allow load explicitly." << endl;
    return 8195;
}

