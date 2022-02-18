#include <iostream>


#include "c11_using.h"

#include "c11_enum_class.h"
#include "c11_other_test.h"
#include "c11_lambda_op.h"
#include "c11_constexpr.h"

#include "move/c11_move.h"
#include "move/c11_swap.h"

#include "perfect_forward/c11_perfect_forward.h"

#include "template/c11_template_test.h"

#include "class/c11_explicit_class.h"
#include "class/c11_mutable_class.h"
#include "class/c11_class_dd.h"
#include "class/c11_rule_five.h"

#include "inherit/c11_final_class.h"
#include "inherit/c11_virtual_inherit.h"


#include "LR_value/c11_LR_value.h"
#include "initial/c11_initial.h"

#include "c11_range_for.h"


#include <vector>


int main()
{
    std::cout << "This is test.\n";

    c11::range_for_test_1();
    c11::range_for_test_2();
    
    c11::rule_five_entry();
    c11::test_mutable_class();
    c11::test_explicit_class();
    c11::class_dd_funcs();
    c11::test_final_class();

    c11::test_virtual_inherit();
    c11::uniform_initial_test();
    c11::initial_list_test();
    c11::vector_initial_test();

    c11::LR_test_func();
    c11::move_entry();
    c11::swap_test();  

    c11::pf_test_funcs();
    c11::template_test_func();
    //c11::test_constexpr_func(10);

    c11::lambda_test_func();
    c11::using_example_1();
    c11::using_example_2();
    c11::enum_class_test();

    c11::other_test_1();
    c11::test_new();
    c11::error_handle_test();

    return 0;
}

