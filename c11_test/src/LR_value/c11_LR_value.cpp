#include "c11_LR_value.h"
#include <iostream>
#include "lr_func.h"


using namespace std;






namespace c11 {






// left value, right value
// http://eli.thegreenplace.net/2011/12/15/understanding-lvalues-and-rvalues-in-c-and-c
void LR_value_test_1()
{
    // left value
    int i = 42;
    int *p = &i; // i is left value
    cout << *p << endl;

    int *p2 = &left_value(); // left_value() is left value.
    *p2 = 55;
    cout << left_value() << endl; // l_value is 55 now.

    int *p3 = new int[3]{1,2,3};
    cout << *(p3 + 1) << endl; // p3+1 is right value, *(p3+1) is left value.
                               //int **pp = &(p3+1); // compile fail. because p3+1 is right value.

    // right value
    //int *p3 = &right_value(); // compile fail. it is right value.
    int j = 925;  // 925 is right value.  (tmp value)
    int a = 3, b = 4; // a, b is left value.
    int c = a + b; // a+b is right value.
    cout << j << " " << c << endl;
    //int *q = &(j+1); // compile error. j+1 is right value.

    //
    std::string str = "abc";
    std::string& str2 = str; // left value reference
                             //std::string& str3 = "abc";  // compile fail. right value can't be referenced.
    const std::string& str3 = "abc"; // success. right value can be reference by const .
    cout << str2 << " " << str3 << endl;



    // reference
    int r = 10;
    //int &r1 = 3; // compile fail.
    int &r2 = r;
    //int &&r3 = r; // compile fail.
    int &&r4 = 3;

    cout << r2 << " " << r4 << endl;
    r2++; r4++;
    cout << r << " " << r4 << endl;

    int rr1 = 3, rr2 = 4;
    //int &rr3 = rr1 + rr2; // compile fail.
    int &&rr4 = rr1 + rr2;
    cout << rr4 << endl;
    rr4 += 1000;
    cout << rr4 << " " << rr1 << " " << rr2 << " " << endl;


    // function parameter
    ref_test_1( r );
    //ref_test_1( 3 ); // compile fail

    ref_test_2( r );
    ref_test_2( 3 );

    //ref_test_3( r ); // compile fail.
    ref_test_3( 3 );

    cout << r << endl;

    int rrr1 = ref_test_4();
    //int &rrr2 = ref_test_4(); // compile fail.
    const int &rrr3 = ref_test_4();
    int &&rrr4 = ref_test_4();

    cout << rrr1 << " " << rrr3 << " " << rrr4 << endl;
}












void LR_value_test_2()
{
    lr_test_f1(10);

    int a = 55;
    //lr_test_f1(a); // build fail.

    lr_test_f2(20);             // 右值參考傳入
    lr_test_f2( int{30} );

    lr_test_f2(a);              // 此時變左值
    cout << a << endl;
    /*
        lr_test_f2 是樣板函數
        出現了傳入變數有機會是左值也有機會是右值的問題.
        衍生討論可以參考perfect forward
    */

}








void LR_test_func()
{
    LR_value_test_1();
    LR_value_test_2();
}





} // end namespace c11