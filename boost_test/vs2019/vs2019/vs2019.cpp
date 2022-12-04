#include <stdlib.h>

#include "json.h"
#include "thread.h"
#include "algorithm.h"
#include "uuid.h"
#include "hardware.h"
#include "container.h"
#include "timer.h"
#include "b_process.h"


int main()
{
    crack_zip();

    /*timer_test();

    boost_json_example();
    thread_test();
    all_of_example();

    find_not_example();
    generate_uuid_example();

    get_cpu_core_func();

    test_vector_emplace();*/

    // check x64 or x86
#if __GNUC__
#if __x86_64__ || __ppc64__
    cout << "this is x64\n";
#else
    cout << "this is x86\n";
#endif
#endif


	return 0;
}

