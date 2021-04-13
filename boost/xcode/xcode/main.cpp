#include "../../algorithm.h"
#include "../../json.h"
#include "../../thread.h"
#include "../../uuid.h"


int main(int argc, const char * argv[])
{
    all_of_example();
    
    boost_json_example();
    
    thread_test();
    
    uuid_test();
    
    return 0;
}
