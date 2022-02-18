#include "c11_move.h"
#include <string>
#include <vector>
#include <iostream>


#include "move_func.h"
#include "move_class_1.h"
#include "move_class_3.h"



using namespace std;







namespace c11 {




void move_test_1()
{
    string str = "hello";
    vector<string> vec;

    vec.push_back(str);
    cout << vec[0] << " str = " << str << endl;

    vec.push_back( move(str) );
    cout << vec[1] << " str = " << str << endl;
}






void move_test_2()
{
    //C2 &c2 = C2(1000);
    Move_1 c2a = Move_1(1000);
    Move_1 c2b = std::move( Move_1(2000) );
    cout << c2b.ptr[0] << endl;
    //cout << "c2a addr = " << &c2a << endl;

    Move_1 c2c = C2_test_1( c2a ); // run constructor
    Move_1 c2d = std::move( C2_test_1(c2b) );
    //cout << "c2b addr = " << &c2b << endl;
    c2c = C2_test_1( c2a );
    cout << c2c.ptr[0] << endl;

    c2c = c2a; // call copy assignment =
    cout << c2c.ptr[0] << endl;

    c2c = Move_1(321);  // call move assignment.
    cout << c2c.size << endl;

    /*
    if we don't define move assign, it will call assign.
    if we define assign as C2& operator = ( C2& _c ) without const, it will compile error.
    */
}







// note: 繼承關係參考 http://en.cppreference.com/w/cpp/language/move_assignment
void move_test_3()
{
    Move_2 d1(1000), d2(1500);

    Move_2 d3( d1 + d2 );
    //Move_2 d3( std::move(d1+d2) );
    cout << d1.ptr[0] << " " << d2.ptr[0] << " " << d3.ptr[0] << endl;

    /*
    this code will not run into move constructor.
    I gauss the reason is RVO.
    visual studio 2019環境下,會跑進move constructor了.
    */
    Move_2 d4( move(d1+d2) );
    cout << d4.ptr[0] << endl;

    Move_2 dd1 = std::move(d1);
    cout << d1.size << " " << dd1.size << endl;

    Move_2 dd2 = move_ct_test_1(d2);
    cout << dd2.ptr[0] << endl;

    Move_2 dd3 = move_ct_test_1( Move_2(234) );
    cout << dd3.size << endl;

    Move_2 dd4 = move_ct_test_2(d2); // see comment move_ct_test_2
    cout << dd4.ptr[0] << endl;

    //Move_2 dd5 = move_ct_test_2( Move_2(111) ); // compile fail.

    //Move_2 dd6 = move_ct_test_3( d2 ); // compile fail.
    Move_2 dd7 = move_ct_test_3( Move_2(323) );
    cout << dd7.ptr[0] << endl;

    Move_2 dd8 = move_ct_test_4( 1000 );
    cout << dd8.ptr[0] << endl;
}






void move_test_4()
{
    Move_3 f1;
    cout << f1.v[0] << endl;

    Move_3 f2(f1);
    cout << f2.v[0] << endl;

    Move_3 f3(move(f1));
    cout << f3.v[0] << " " << f1.v.size() << endl;

    // note: if we don't use std::move, it will not call move constructor.
    // 原因不確定是否跟 RVO 有關 
    Move_3 f4 = move(Move_3{100});    
    cout << f4.v[0] << " " << f4.v.size() << endl;
}



void move_entry()
{
    c11::move_test_1();
    c11::move_test_2();
    c11::move_test_3();
    c11::move_test_4();
}




} // end namespace c11
