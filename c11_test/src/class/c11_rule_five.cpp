#include "c11_rule_five.h"
#include "rule_five_class.h"
#include <iostream>


using namespace std;




namespace c11 {


void test_rule_five_1()
{
    Array a(10);
    Array b{a};

    a.print();
    b.print();

    Array c = a + b;
    c.print();

    c = a + b; // 進入move assignment
    c.print();
}



void test_rule_five_2()
{
    Array a(10);
    
    Array b = add_5_func(a); // d 進入 move constructor.
    b.print();

    Array c;
    c = add_5_func( a+b ); // vs2019會進move constructor. 推測是優化造成的.
    c.print();

    //Array d = add_10_func(a); // 編譯錯誤
    Array d = add_10_func(a+b); 
    d.print();
}






void test_rule_five_3()
{
    // case 1
    Array a(10), b(10), c(10), d(10), e(10);

    /* 
        這個版本不會進copy constructor
        note: xcode, vs2019執行結果不同.
        compile optimized結果有機會發生差異
    */
    Array sum = a + b + c + d + e;
    sum.print();

    sum = a + b + c + d + e;
    sum.print();
}




void test_rule_five_4()
{
    Array a(10), b(10);

    auto ptr = make_instance_2<Array>(a);
    ptr->print();

    auto ptr2 = make_instance_2<Array>( Array{10} );
    ptr2->print();

    auto ptr3 = make_instance_2<Array>( a + b );
    ptr3->print();

    a.print();
    b.print();

    /*
        make_instance_2 沒加 forward, 創建unique ptr會用copy constructor, 複製資料, 浪費時間
        make_instance   使用 forward, 創建unique ptr會根據情況使用move constructor, 效能比較好.
    */

    auto ptr4 = make_instance<Array>(a);
    ptr4->print();

    auto ptr5 = make_instance<Array>( Array{10} );
    ptr5->print();

    auto ptr6 = make_instance<Array>( a + b );
    ptr6->print();

    a.print();
    b.print();

}





void rule_five_entry()
{
    test_rule_five_1();
    test_rule_five_2();
    test_rule_five_3();
    test_rule_five_4();
}





} // end namespace c11



