//
//  main.cpp
//  xcode
//
//  Created by hidog on 2014/9/24.
//  Copyright (c) 2014年 hidog. All rights reserved.
//

#include <iostream>
#include <boost/thread.hpp>

int     f( int thr_id )
{
    for( int i = 0; i < 10; i++ )
        std::cout << "thr = " << thr_id << std::endl;
    return  10;
}

int main(int argc, const char * argv[])
{
    // insert code here...
    std::cout << "Hello, World!\n";
    
    
    boost::thread   thr[10];
    for( int i = 0; i < 10; i++ )
        thr[i]  =   boost::thread( &f, i );
    for( int i = 0; i < 10; i++ )
        thr[i].join();
    
    
    return 0;
}
