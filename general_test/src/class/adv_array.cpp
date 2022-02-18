#include "adv_array.h"
#include <iostream>
#include <stdlib.h>

using namespace std;




AdvArray::AdvArray() 
    : Array(20)
{
    cout << "AdvArray(), addr = " << this << endl;
}







AdvArray::~AdvArray()
{
    cout << "~AdvArray(), addr = " << this << endl;  
}



void AdvArray::output()
{
    cout << "AdvArray::output\n";
}



void AdvArray::output2()
{
    cout << "AdvArray::output2\n";
}
