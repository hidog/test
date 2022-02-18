#include "c11_initial.h"
#include "initial_obj.h"

#include <iostream>
#include <vector>

using namespace std;



namespace c11 {


void uniform_initial_test()
{
    // initialized, and
    int i;
    
    // Uniform initialization
    int a1 = {-5};
    cout << "a1 = " << a1 << endl;
    
    // 5.44f, 會是float.   5.44, vs2019, 被當double, 跳出warning.
    float a2{5.44f};
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
    //int a7 = static_cast<int>(2.34); // 消warning.
    cout << a7 << endl;
}





void initial_list_test()
{
    int i;

    InitialObj a1;
    for( i = 0; i < 5; i++ )
        cout << a1.ptr[i] << " ";
    cout << a1.data1 << " " << a1.data2 << endl;

    InitialObj a2(3,4);   // c99
    cout << a2.data1 << " " << a2.data2 << " " << a2.b3.d << endl;

    // if use initialer_list in constructor, a3 and a4 will run A(initializer_list), not A(int,int)
    InitialObj a3{ 2, 3 };
    cout << a3.data1 << " " << a3.data2 << endl;

    InitialObj a4 = { 36, -53 };
    cout << a4.data1 << " " << a4.data2 << endl;

    InitialObj a5( {-1,2,0,4} );
    //InitialObj a5{ {-1,2,0,4} }; 可以使用大括號
    for( i = 0; i < 4; i++ )
        cout << a5.ptr[i] << " ";
    cout << endl;

    //int aaa = nullptr; // compile fail.
    int bbb = NULL; // this is difference between nullptr and NULL.
    cout << bbb << endl;

}




void vector_initial_test()
{
    /*
        請注意 veor<int> v1{3} 跟 vector<int> v1(3) 的不同
        vector<int> v1{3} 會進去 initialized list, 會創造出只有一個物件的vector,內容為 3
        vector<int> v1(3) 會進去 vector( int n ), 創造出三個物件的vector.
    */
    vector<int> v1{3};
    cout << "v1 size = " << v1.size() << endl;
    for( auto itm : v1 )
        cout << itm << " ";
    cout << endl << endl;

    vector<int> v2(3);
    cout << "v2 size = " << v2.size() << endl;
    for( auto itm : v2 )
        cout << itm << " ";
    cout << endl << endl;

    vector<int> v3{ 2, 3, 4, 5, 6, 7 };
    cout << "v3 size = " << v3.size() << endl;
    for( auto itm : v3 )
        cout << itm << " ";
    cout << endl << endl;

}



} // end namespace c11