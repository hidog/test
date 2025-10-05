#include <iostream>

#include "hardware.h"
#include "json.h"
#include "thread.h"
#include "algorithm.h"
#include "uuid.h"
#include "container.h"
#include "b_process.h"


using namespace std;
 


int main()
{
    //process_example();
    unrar_main();

    //boost_json_example();    
    //thread_test();    
    //all_of_example();

    //find_not_example();
    //generate_uuid_example();
    
    //get_cpu_core_func();

    //test_vector_emplace();
    
// check x64 or x86
#if __GNUC__
#if __x86_64__ || __ppc64__
    cout << "\n\nthis is x64\n\n";
#else
    cout << "\n\nthis is x86\n\n";
#endif
#endif

    return 0;
}
