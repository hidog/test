#ifndef MOVE_CLASS_1_H
#define MOVE_CLASS_1_H



class Move_1
{
public:
    Move_1();
    Move_1( int _s );
    Move_1( const Move_1& _c );
    Move_1( Move_1&& _c ) noexcept;

    ~Move_1();

    Move_1& operator = ( const Move_1& _c );    
    Move_1& operator = ( Move_1&& _c ) noexcept;  // move assignment

    int size;
    int *ptr;
};





#endif