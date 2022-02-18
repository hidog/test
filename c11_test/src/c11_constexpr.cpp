#include "c11_constexpr.h"
#include <iostream>

using namespace std;




namespace c11 {



constexpr int test_cpr_1()
{
    return 44;
}


constexpr int test_cpr_2( int a )
{
    return a + 10;
}





/*
    constexpr 跟 const的差異
    
    constexpr 屬於編譯期決定
    不能輸入變數
    好處是因為編譯期決定,效率較佳.
    如果該值可由編譯器推導出來,則該語法亦可以通過.

    const是該variable不能改變內容,但可以是任意變數作為初始值.
*/
int test_constexpr_func( int i )
{
    //constexpr int a = i+1;  編譯不過
    constexpr int a = 3;
    const int b = i + 1;
    

    constexpr int c = test_cpr_1();
    int d = test_cpr_1();  // 會有warning
    cout << "c = " << c << ", d = " << d << endl;


    int input;
    cout << "enter a number : ";
    cin >> input;
    int e = test_cpr_2(input);              // e 宣告成普通的變數, 此時不受限制能編譯.
    //constexpr int f = test_cpr_2(input);  // 會出現編譯錯誤
    constexpr int f = test_cpr_2(445);      // 可以從編譯器推導,編譯成功
    cout << "e = " << e << ", f = " << f << endl;


    /* 
        constexpr 通常會配合 constexpr function一起使用. 
        當值可以推導的時候,可以編譯時期決定值,讓效能比較好
        類似macro的效果,但維護性比macro好.
    */

    return a*b;
}





} // end namespace c11
