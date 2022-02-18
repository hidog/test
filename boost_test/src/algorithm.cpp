#include "algorithm.h"
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/find_not.hpp>
#include <boost/container/vector.hpp>
#include <iostream>


bool is_odd( int i )
{
    return i % 2 == 1;
}





void all_of_example()
{
    bool flag;
    
    boost::container::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7 };    
    flag = boost::algorithm::all_of( vec, is_odd );    
    if( flag == true )
        std::cout << "vec is all odd\n";
    else
        std::cout << "vec is not all odd\n";
        
    flag = boost::algorithm::all_of( vec, [](int i) { 
                                        return i < 10; 
                                     } );
    if( flag == true )
        std::cout << "vec all less than 10\n";
    else
        std::cout << "vec not all less than 10\n";

}




void find_not_example()
{
    boost::container::vector<int> vec = { 2, 3, 4, 5 };
    auto itr = boost::algorithm::find_not( vec.begin(), vec.end(), 2 );
    std::cout << *itr << std::endl;
}
