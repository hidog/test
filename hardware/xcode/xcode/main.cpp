
#include "CPU.h"

#include <iostream>

using namespace std;

int main()
{
    cout << "get core = " << cpu::get_real_cpu_core() << "\n\n";
    cout << "get core = " << cpu::get_logical_cpu_core() << "\n\n";

    cout << "vendor = " << cpu::get_cpu_vender() << "\n\n";
    cout << "cpu id = " << cpu::get_cpu_serial() << "\n\n";
    cout << "cpu id = " << cpu::cpu_get_serial_asm() << "\n\n";

    return 0;
}

