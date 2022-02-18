#include <iostream>


#include "CPU.h"
#include "timer.h"
#include "file.h"
#include "macro.h"

#include "thread/thread_entry.h"


using namespace std;



int main()
{
    test_cpu_info_func();
    test_timer_func();

    uint64_t file_size = get_file_size();
    cout << "file size = " << file_size << "bytes, " << endl
         << 1.f*file_size/1024 << "kb, " << endl
         << 1.0*file_size/1024/1024 << "mb" <<endl;

    x64_x86_macro();
    c11_macro();
    c_thread_test();

    return 0;
}

