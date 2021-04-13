
#include "../../thread.h"
#include "../../json.h"
#include "../../algorithm.h"
#include "../../uuid.h"


int main(int argc, const char * argv[])
{
    thread_test();
    
    boost_json_example();
    
    all_of_example();
    find_not_example();
    
    generate_uuid_example();
    
    return 0;
}
