#ifndef TEMPLATE_FUNC_H
#define TEMPLATE_FUNC_H

#include <iostream>
#include <vector>

using namespace std;


// 可以自動決定回傳型別
template<typename T1, typename T2>
auto operator + ( vector<T1> t1, vector<T2> t2 ) -> vector<decltype(T1{}+T2{})>
{
    size_t min_size = t1.size() < t2.size() ? t1.size() : t2.size();
    vector<decltype(T1{}+T2{})> sum(min_size);

    for( size_t i = 0; i < min_size; i++ )
        sum[i] = t1[i] + t2[i];

    return sum;
}



template<typename Head>
void v_func( Head head )
{
    cout << head << " ";
}


template<typename Head, typename... Tail>
void v_func( Head head, Tail... tail )
{
    // 動態變數的樣板寫法. 
    v_func(head);
    v_func(tail...);
}




template<typename T>
T sum(const T& first)
{
    return first;
}


template<typename T, typename... Args>
T sum(const T& first, const Args&... args)
{
#if defined(MACOS) || defined(UNIX)
    // 原本的寫法,會產生warning.
    // mac自動捕捉型態的寫法還沒研究出來
    return first + sum(args...);
#else
    // 動態變數的樣板
    // 用decltype動態轉換回傳型態,消去warning.
    auto result =  first + sum(args...);
    return static_cast<decltype(T{}+Args{})>(result);
#endif
}





template<int T1=10>
void tmplate_print()
{
    printf("%d ", T1 );
    tmplate_print<T1-1>();
}



template<>
void tmplate_print<1>()
{
    printf("%d\n", 1 );
}






#endif
