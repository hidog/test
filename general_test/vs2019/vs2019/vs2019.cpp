#include <iostream>

#include "linklist.h"
#include "pointer.h"
#include "variable_arg.h"
#include "macro.h"
#include "typeid.h"
#include "template.h"
#include "other_test.h"
#include "seperate_large_file.h"

#include "class/rule_three.h"
#include "class/inherit.h"
#include "scanf_regular.h"

#include "cast/cast_test.h"


using namespace std;




int main()
{
    //test_seperate2();

    //slf_test();

    SlfSetting setting;

#if 0
    sprintf( setting.filename, "test2.rar" );
    setting.prime = 101;
    setting.block_size = 10000;
    seperate_large_file( "I://temp//", "J://test//", setting );
#else
    int res = slf_load_info( "J:\\test\\", setting );
    if( res < 0 )
    {
        printf("error\n");
        return 0;
    }
    setting.prime = 101;
    merge_large_file( "J:\\test\\", "I:\\temp\\", setting );
#endif

    //write_disk_to_full_test( "I:\\test.dat" );
    return 0;


    cast_test_func();

    bit_field_test();
    ref_test();
    raw_string_test();

    cout << template_func_1(34.22) << endl;
    cout << template_func_1(44) << endl;

    test_typeid_1();
    test_typeid_2();
    cout << endl;

    test_macro_func();
    linklist_test();
    test_pointer();
    test_rule_three();
    va_test( 5, 100, 200, 300, 400, 500 );
    cout << endl;

    test_inherit_1();
    //test_inherit_2();
    test_inherit_3();
    cout << endl;

    cstyle_regular_expression_func();
    

    return 0;
}

