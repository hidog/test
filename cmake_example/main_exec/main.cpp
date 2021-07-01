#include "static.h"
#include "dynamic.h"
#include <cstdlib>


int main()
{
    static_test();

    dynamic_test();

    explicit_load_func();

    system("PAUSE");
    return 0;
}
    