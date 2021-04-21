
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <iostream>


using namespace std;
 
void f()
{
    int i;
    for( i = 0; i < 10; i++ )
        cout << i << " ";
}



void thread_test()
{
    boost::thread t[10];
    for( int i = 0; i < 10; i++ )
        t[i] = boost::thread(&f);
    for( int j = 0; j < 10; j++ )
        t[j].join();


    boost::array<int, 4> arr = {{1,2,3,4}};
    cout << "hi" << arr[0];
}
