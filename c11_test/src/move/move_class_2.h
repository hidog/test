#ifndef MOVE_CLASS_2_H
#define MOVE_CLASS_2_H


class Move_2
{
public:
    Move_2();
    Move_2( int _s );
    Move_2( const Move_2& _d );   
    Move_2( Move_2&& _d ) noexcept; // move constructor

    ~Move_2();

    Move_2 operator + ( const Move_2& _d );

    int size;
    int *ptr;
};


#endif