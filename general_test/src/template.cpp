#include "template.h"


template<typename T>
T template_func_1( T input )
{
    return 3*input + 100;
}


// function 樣板具現化 Instantiate 
// visual studio環境下有跳警告, 目前找不到好的解決方式
template double template_func_1<double>(double);
template int template_func_1<int>(int);