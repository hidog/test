#ifndef TEST_CLASS_4_H
#define TEST_CLASS_4_H

#include <vector>


class F
{
public:
    F() : v{1,2,3,4,5,6,7,8,9}
    {}

    F( int n )
    {
        v.resize(n);
        for( int i = 0; i < n; i++ )
            v[i] = rand() * 100;
    }

    F( const F& f )
    {
        v   =   f.v;
        v[0]++;
    }

    F( F&& f )
    {
        v   =   move(f.v);
        v[0] = -v[0];
    }

    ~F(){}

    std::vector<int> v;
};




#endif