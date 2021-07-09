#include "c11_initial.h"
#include <iostream>

using namespace std;



namespace c11 {


void uniform_initial_test()
{
    // initialized, and
    int i;
    
    // Uniform initialization
    int a1 = {-5};
    cout << "a1 = " << a1 << endl;
    
    //
    float a2{5.44};
    cout << "a2 = " << a2 << endl;
    
    //
    unsigned int a3(5);  // c99
    cout << "a3 = " << a3 << endl;
    
    //
    double a4[5]{1.1, 2.2, 3.3, 4.4};
    for( i = 0; i < 5; i++ )
        cout << a4[i] << " ";
    cout << endl;
    
    //
    char a5[5] = { 'a', 'b', 'c', 'd', 'e' };   // c99
    for( i = 0; i < 5; i++ )
        cout << a5[i] << " ";
    cout << endl;
    
    //
    int *ptr = new int[3]{ 5, 9, 3 };
    for( i = 0; i < 3; i++ )
        cout << ptr[i] << " ";
    cout << endl;
    
    //
    //int a6{2.34};   // compile error. can't convert double to int.
    int a7 = 2.34;    // ok. a7 = 2   convert double to int
    cout << a7 << endl;
}



} // end namespace c11