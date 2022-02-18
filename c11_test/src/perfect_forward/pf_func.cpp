#include "pf_func.h"
#include <iostream>

 using namespace std;





 std::unique_ptr<PF_A> pf_test_4( int&& u )
 {
     return std::unique_ptr<PF_A>( new PF_A( std::forward<int>(u) ) );
 }



 std::unique_ptr<PF_A> pf_test_5( const int& u )
 {
     return std::unique_ptr<PF_A>( new PF_A( std::forward<const int&>(u) ) );
 }




void pf_test_6( int && a )
{
#if 0
    if( a == 0 )
    {
        cout << "\n";
        return;
    }
    else
    {
        cout << a << ", ";
        --a;
        //pf_test_6(a);  // 編譯錯誤,這邊的 a 是左值
        pf_test_6(a-1);   // a-1是右值,編譯通過.
    }
#elif 1
    if( a == 0 )
    {
        cout << endl;
        return;
    }
    else
    {
        cout << a << ", ";
        --a;
        pf_test_6( std::forward<int>(a) );  // 這邊編譯通過. 加上forward能保持左值或右值性質. (perfect forward)
    }
#endif
}



void pf_test_8( PF1&& pf )
{
    pf.output();
    //pf_test_9(pf);  // 編譯錯誤,因為pf變成左值
    pf_test_9( forward<PF1>(pf) );  // 編譯成功
}



void pf_test_9( PF1&& pf )
{
    pf.output();
}


