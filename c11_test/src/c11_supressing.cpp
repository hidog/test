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






} // end namespace 





namespace c11 {



void supressing_operator_test()
{
    cout << "supressing_operator_test" << endl;
    Supressing s1, s2;
    //s1 = s2; // ·|¾É­P½sÄ¶¿ù»~
}




} // end namespace c11 