#ifndef INITIAL_BASE_H
#define INITIAL_BASE_H


class InitialBase
{
public:
    InitialBase( int _a );
    InitialBase( int _a, double _d );

    int a = 4;
    //int b(3);  // compile fail.
    int c{4};
    double d;
};




#endif