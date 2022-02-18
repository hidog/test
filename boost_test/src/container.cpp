#include "container.h"

#include <boost/container/vector.hpp>
#include <string>
#include <iostream>


class MyClass
{
public:
    MyClass( std::string str, int a, double b )
    {
        name    =   str;
        age     =   a;
        height  =   b;
    }




    std::string name;
    int age;
    double height;  
};



std::ostream& operator << ( std::ostream& os, const MyClass& c )
{
    os << "name = " << c.name << "\n"
       << "age = " << c.age << "\n"
       << "height = " << c.height << "\n";
    return os;
}






void test_vector_emplace()
{
    boost::container::vector<MyClass>   vec;

    vec.emplace_back( std::string{"John"}, 20, 170.2 );
    std::cout << vec[0];
}
