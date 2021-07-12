#include "c11_supressing.h"
#include <iostream>

using namespace std;



namespace {


    
    
class Supressing
{
public:
    Supressing() {}
    Supressing( const Supressing& s ) = delete;
    Supressing( Supressing&& s ) = delete;
    Supressing& operator = ( Supressing&& s ) = delete;
    Supressing& operator = ( const Supressing& s ) = delete;
    ~Supressing() {}

private:


};




class Supressing_2
{
public:
    Supressing_2() : data{10} {}

    Supressing_2( const Supressing_2& s ) 
    { 
        data = s.data + 1; 
    }
    
    Supressing_2( Supressing_2&& s ) = delete;

    Supressing_2& operator = ( Supressing_2&& s ) = delete;

    Supressing_2& operator = ( const Supressing_2& s )
    {
        data = s.data + 1;
        return *this;
    }

    ~Supressing_2() {}

    void print() 
    {
        cout << "data = " << data << endl;
    }

private:

    int data;


};


#if 1
Supressing_2& test_supressing_2( Supressing_2 a )
{
    return a; 
}
#else
Supressing_2 test_supressing_2( Supressing_2 a )
{
    // move constructor 設為 delete, 會造成這個函數編譯錯誤.
    return a; 
}
#endif


} // end namespace 





namespace c11 {



void supressing_operator_test()
{
    cout << "supressing_operator_test" << endl;
    Supressing s1, s2;
    //s1 = s2; // 會導致編譯錯誤
}


void supressing_operator_test_2()
{
    Supressing_2 s;
    Supressing_2 s2;
    s2 = test_supressing_2(s);
    s2.print();
}




} // end namespace c11 