#include "c11_perfect_forward.h"
#include "pf_func.h"
#include <iostream>
#include "pf_class.h"


using namespace std;



namespace c11 {


void perfect_forward_test_1()
{
    auto p = pf_test_1<PF_A>(10);

    // official sample
    auto p1 = pf_test_2<PF_A>(2); // rvalue
                                //auto p1 = make_unique_1(2); // compile error
                                //unique_ptr<AA> p1 = make_unique_1(3); // compile error

    int i = 1;
    auto p2 = pf_test_2<PF_A>(i); // lvalue



    std::cout << "B\n";
    auto t1 = pf_test_3<PF_B>(2, i, 3);
    t1->output();

    auto t2 = pf_test_3<PF_B>( 3.52, 100, 'A', "this is test.\n" );
    t2->output();



    auto p3 = pf_test_4(3);
    //auto p4 = pf_test_4(i); // compile fail.
    auto p5 = pf_test_5(i);
}





void perfect_forward_test_2()
{
    PF  pf = { PF1(), PF2() };
    cout << endl;

    PF1 pf1;
    PF2 pf2;
    PF pf_a{ pf1, pf2 };
    cout << endl;

    PF pf_b( PF1(), pf2 );
}





void perfect_forward_test_3()
{
    pf_test_6(10);

    int a = 10;
    //pf_test_6(a);     // 編譯錯誤
    //pf_test_6(++a);   // 編譯錯誤
    pf_test_6( a-5 );
}








void perfect_forward_test_4()
{
    pf_test_7(10);

    int a = 10;
    pf_test_7(a);           // 改樣板,同時可以支援左值或右值
    cout << "a = " << a << endl; // a = 0, 因為用左值參考傳入

    int b = 10;
    pf_test_7(++b);  
    cout << "b = " << b << endl; // b =0, 因為用左值參考傳入

    int c = 10;
    pf_test_7( c-5 );
    cout << "c = " << c << endl; // c = 10, 因為用右值參考傳入
}




void perfect_forward_test_5()
{
    pf_test_8( PF1{} );

    PF1 pf;
    PF1 pf2 = move(pf);
    //pf_test_8(pf); // 編譯錯誤
    pf_test_8( move(pf2) );

}




void perfect_forward_test_6()
{
    pf_test_10( PF1{} );

    PF1 pf;
    PF1 pf2 = move(pf);
    pf_test_10(pf); // 樣板可同時吃左值跟右值
    pf_test_10( pf2 + pf );
}





void pf_test_funcs()
{
    perfect_forward_test_1();
    perfect_forward_test_2();
    perfect_forward_test_3();
    perfect_forward_test_4();
    perfect_forward_test_5();
    perfect_forward_test_6();
}





} // end namespace c11