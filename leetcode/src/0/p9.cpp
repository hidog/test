#include "p9.h"

#include <iostream>
#include <cstdint>

using namespace std;



bool isPalindrome(int x) 
{        
    if( x < 0 )
        return false;
    
    int64_t y = x, z = 0;
    while( y > 0 )
    {
        z = z*10 + y%10;
        y /= 10;            
    }
    if( z == x )
        return true;
    else
        return false;
}



void    p9_PalindromeNumber()
{
    cout << isPalindrome(2147483647) ? "yes" : "no";
}


