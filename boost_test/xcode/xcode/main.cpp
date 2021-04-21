#include "../../thread.h"
#include "../../json.h"
#include "../../algorithm.h"
#include "../../uuid.h"

#include <stdio.h>


int main(int argc, const char * argv[])
{
    thread_test();
    
    boost_json_example();
    printf("\n");
    
    all_of_example();
    find_not_example();
    
    generate_uuid_example();
    
    return 0;
}
