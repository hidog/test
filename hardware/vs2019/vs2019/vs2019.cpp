
#include "CPU.h"

#include <iostream>

using namespace std;

int main()
{
    cout << "get core = " << get_real_cpu_core() << "\n\n";
    cout << "get core = " << get_logical_cpu_core() << "\n\n";

    cout << "vendor = " << cpu_get_vender() << "\n\n";
    cout << "cpu id = " << cpu_get_serial() << "\n\n";

    system("PAUSE");
}

