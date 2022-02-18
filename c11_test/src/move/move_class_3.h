#ifndef MOVE_CLASS_3_H
#define MOVE_CLASS_3_H

#include <vector>


class Move_3
{
public:
    Move_3();
    Move_3( int n );
    Move_3( const Move_3& f );
    Move_3( Move_3&& f );
    ~Move_3();

    std::vector<int> v;
};




#endif
