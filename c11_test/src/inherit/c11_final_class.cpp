#include "c11_final_class.h"

#include <iostream>

using namespace std;





void FinalBase::f1()
{
    cout << "Base f1\n";
}



void FinalBase::f2()
{
    cout << "Base f2\n";
}



void FinalBase::f3() 
{
    cout << "Base f3\n";
}



void FinalBase::f4( int a )
{
    cout << a << "   Base f4\n";
}



void FinalObj::f4(double a)
{
    cout << 6.2145*a << "   Obj f4\n";
}




void FinalObj::f1()
{
    cout << "Obj f1\n";
}



void FinalObj::f2() 
{
    cout << "Obj f2\n";
}







namespace c11 {





void test_final_class()
{
    //
    FinalBase* ptr = new FinalObj;
    ptr->f1();
    ptr->f2();

    FinalObj* ptr2 = new FinalObj;
    ptr2->f4(2);
}






} // end namespace c11