#include "smart_point.h"
#include "test.h"

#include <memory>


void test_unique_point()
{
    std::unique_ptr<Test> uni_ptr( new Test);    
    uni_ptr->print();
}
