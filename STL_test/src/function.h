#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <iostream>
#include <vector>
#include <list>



class TargetObj
{
public:
    TargetObj() = default;
    ~TargetObj() = default;
    
    std::string obj_func( int x, int y );
    
    int a = 10;
    double b = 351.552;
};


int target_func( int a, char b, std::string c, double d );
void ref_func_add_10( int& a, std::string& b );


void test_less_func();
void test_bind();





std::ostream& operator << ( std::ostream& os, std::vector<double> c );
std::ostream& operator << ( std::ostream& os, std::list<int> c );



void test_functions();
void test_pair();
void test_tuple();


#endif
