#include "atomic_test.h"

#include <atomic>
#include <iostream>

using namespace std;



void atomic_test()
{
    std::atomic<int> a { 10 };
    std::cout << "a = " << a << "\n";

    a += 10;
    std::cout << "a = " << a << "\n";

    atomic_int b { 5151 };
    cout << "b = " << b << endl;

    std::cout << std::boolalpha
              << "std::atomic<A> is lock free? "
              << std::atomic<int>{}.is_lock_free() << '\n'
              << "std::atomic<B> is lock free? "
              << std::atomic<double>{}.is_lock_free() << '\n';
}
