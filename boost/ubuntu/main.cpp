#include <iostream>
#include "json.h"
#include "thread.h"
#include "algorithm.h"


using namespace std;
 


int main()
{
    boost_json_example();
    
    thread_test();
    
    all_of_example();
    
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
