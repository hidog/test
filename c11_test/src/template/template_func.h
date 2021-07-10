#ifndef TEMPLATE_FUNC_H
#define TEMPLATE_FUNC_H

#include <iostream>

using namespace std;



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