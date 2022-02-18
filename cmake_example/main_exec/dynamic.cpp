#include "dynamic.h"

#include "dynamic_lib_1.h"
#include "dynamic_lib_2.h"

extern "C" {
#include "dynamic_lib_3.h"
}


#include <iostream>




using namespace std;





void dynamic_test()
{
    int ret;
    
    //
    ret = dynamic_func_1();
    cout << "ret = " << ret << "\n\n";

    //
    double ret2 = dynamic_func_2( 481421, 9982157 );
    cout << "ret2 = " << ret2 << "\n\n";

    // 
    dynamic_func_3();
    
    // 
    ret = dynamic_func_4();
    cout << "ret = " << ret << "\n\n";

    

    DynamicObj d_obj;
    d_obj.test_func();

    cout << endl;

}
