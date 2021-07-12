#include <iostream>
#include <cstdlib>
#include "c11_sample.h"
#include "c11_initial.h"
#include "c11_LR_value.h"
#include "rule_three.h"
#include "c11_supressing.h"
#include "c11_using.h"

#include "enum_class.h"
#include "rule_five.h"

#include "move/c11_move.h"
#include "template/c11_template_test.h"



int main()
{
    std::cout << "This is test.\n";

    //c11::uniform_initial_test();
    //c11::initial_list_test();
    //c11::LR_value_test();
    //test_rule_three();
    
    //c11::move_test_1();
    //c11::move_test_2();
    //c11::move_test_3();
    //c11::move_test_4();
    
    //c11_enum_class_test();
    //c11::supressing_operator_test();
    //c11::test_rule_five();

    //c11::test_template();
    //c11::test_func_obj();
    //c11::test_variadic_template();
    
    c11::using_example();
    c11::using_example_2();

    //c11_sample_9(); // 這個執行會造成crash,有時間在修
    //c11_sample_11();
    //c11_sample_12();
    //c11_sample_13();
    //c11_swap();

    system("PAUSE");
    return 0;
}

