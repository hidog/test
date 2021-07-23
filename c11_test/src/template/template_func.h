#ifndef TEMPLATE_FUNC_H
#define TEMPLATE_FUNC_H

#include <iostream>
#include <vector>

using namespace std;


// 可以自動決定回傳型別
template<typename T1, typename T2>
auto operator + ( vector<T1> t1, vector<T2> t2 ) -> vector<decltype(T1{}+T2{})>
{
    int min_size = t1.size() < t2.size() ? t1.size() : t2.size();
    vector<decltype(T1{}+T2{})> sum(min_size);

    for( int i = 0; i < min_size; i++ )
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
    return first + sum(args...);
}



#endif