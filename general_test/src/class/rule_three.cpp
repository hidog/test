#include "rule_three.h"
#include <iostream>
#include "adv_array.h"


using namespace std;





Array array_func_1( Array c )
{
    Array tmp;
    tmp = c;
    tmp.add(100);
    return tmp;
    /*
        note: Return value optimization (RVO),
        實際上編譯器有機會最佳化,降低資料複製的次數.
    */
}






// traditional rule of three.
void test_rule_three()
{
    Array a1(10);
    a1.output();

    Array a2 = a1;
    a2 = array_func_1( a2 );
    a2.output();
}



