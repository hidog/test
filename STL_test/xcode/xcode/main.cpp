
#include <iostream>

#include "smart_pt/smart_point.h"
#include "thread/thread_test.h"

#include "container.h"
#include "function.h"
#include "c11_timer.h"
#include "regular_express.h"
#include "random.h"

#include "other_test.h"
#include "atomic_test.h"




int main()
{
    test_smart_point();
    test_container_func_1();
    test_container_func_2();
    test_container_func_3();
    
    test_functions();
    test_pair();
    test_tuple();
	c11_timer_func();

    test_char_string();
    regular_express_func();
    random_func();
	atomic_test();

    thread_entry_func();
    
    return 0;
}

