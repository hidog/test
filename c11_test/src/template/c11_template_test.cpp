#include "c11_template_test.h"

#include <iostream>
#include "template_func.h"
#include "template_array.h"

using namespace std;








namespace c11{


void test_decltype_template_1()
{
    vector<int> a = { 1, 2, 3, 4, 5 };
    vector<double> b = { 1.3, 6.2, 661.52223 };

    auto c = a + b;
    auto d = b + a;

    cout << "vector c = " << endl;
    for( size_t i = 0; i < c.size(); i++ )
        cout << c[i] << " ";
    cout << "\n\n";

    cout << "vector d = " << endl;
    for( size_t i = 0; i < d.size(); i++ )
        cout << d[i] << " ";
    cout << "\n\n";
}



void test_decltype_template_2()
{
    int a = 10;
    vector<decltype(a)> vec( 5, a );
    
    for( auto d : vec )
        cout << d << " ";
    cout << endl;
}


void test_template_foreach()
{
    TArray<double> a{10};
    a.print();
    for( auto &itr : a )
        cout << itr << " ";
}



void test_func_obj()
{
    TArray<int> a_func(5);
    cout << a_func(3); // a_fun·Q¹³¦¨function
}


void test_variadic_template()
{
    cout << sum( 1, 1.2 );
    v_func( 1, 3, 4.4, "abcde", 'Z' );
}



void test_recursive_template()
{
    tmplate_print<9>();
}






void template_test_func()
{
    test_template_foreach();
    test_func_obj();
    test_variadic_template();
    test_decltype_template_1(); 
    test_decltype_template_2();   
    test_recursive_template();
}




} // end namespace c11
