#ifndef PF_CLASS_H
#define PF_CLASS_H

#include <iostream>
#include <string>



struct PF_A
{
    PF_A() : aa(0) 
    { 
        std::cout << "constructor." << std::endl; 
    }


    PF_A( int&& n ) : aa(n) 
    { 
        std::cout << "rvalue overload, n=" << n << std::endl; 
    }


    PF_A( const int& n ) : aa(n) 
    { 
        std::cout << "lvalue overload, n=" << n << std::endl; 
    }

    int aa;
};






class PF_B
{
public:
    template<class T1, class T2, class T3>
    PF_B(T1&& t1, T2&& t2, T3&& t3) :
        a1_{std::forward<T1>(t1)},
        a2_{std::forward<T2>(t2)},
        a3_{std::forward<T3>(t3)}
    {
        data_1 = 0;
        data_2 = 0;
        data_3 = 0;        
    }


    PF_B( double&& d1, int&& d2, char&& d3, std::string&& d4 ) : 
        data_1{std::forward<double>(d1)},
        data_2{std::forward<int>(d2)},
        data_3{std::forward<char>(d3)},
        data_4{std::forward<std::string>(d4)}
    {}

    // it will cause run lvalue overload.
    /*template<class T1, class T2, class T3>
    BB(T1&& t1, T2&& t2, T3&& t3) :
    a1_{t1},
    a2_{t2},
    a3_{t3}
    {}*/

    void output()
    {
        std::cout << data_1 << ", " << data_2 << ", " << data_3 << ", " << data_4  << std::endl;
    }

private:
    PF_A a1_, a2_, a3_;

    double data_1;
    int data_2;
    char data_3;
    std::string data_4;
};










class PF1
{
public:
    PF1() : data{0}
    { 
        std::cout << "PF1 constructor" << std::endl; 
    }


    PF1( const PF1& pf ) : data{1}
    { 
        std::cout << "PF1 copy constructor" << std::endl; 
    }


    PF1( PF1&& pf ) noexcept : data{2}
    { 
        std::cout << "PF1 move constructor" << std::endl; 
    }

    PF1& operator = ( const PF1& ) = default;
    PF1& operator = ( PF1&& ) = default;

    void output()
    {
        switch(data)
        {
        case 0 :
            std::cout << "default constructor\n";
            break;
        case 1:
            std::cout << "copy constructor\n";
            break;
        case 2:
            std::cout << "move constructor\n";
            break;
        default:
            std::cout << "error\n";
        }
    }

    PF1 operator + ( const PF1& pf )
    {
        PF1 tmp = *this;
        return tmp;
    }

    int data;
};






class PF2
{
public:
    PF2()
    { 
        std::cout << "PF2 constructor" << std::endl; 
    }



    PF2( const PF2& pf )
    { 
        std::cout << "PF2 copy constructor" << std::endl; 
    }



    PF2( PF2&& pf ) noexcept 
    { 
        std::cout << "PF2 move constructor" << std::endl; 
    }
};






class PF
{
public:

    // you don't need define four constructor.
    // I have try for make_pair but fail.
    template<typename T, typename U>
    PF( T&& t, U&& u )
        : p1(std::forward<T>(t)), p2(std::forward<U>(u))
    {}

    PF1    p1;
    PF2    p2;
};











#endif