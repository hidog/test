#include "process.h"

#include <boost/process.hpp>
#include <iostream>
#include <cstring>
#include <vector>

using namespace boost::process;


bool    crack_zip()
{
#if 0
    ipstream pipe_stream;
    child c("J:\\UnRAR.exe x -Pabc \"J:\\test.rar\" \"J:\\tmp\"", std_out > pipe_stream);

    //UNRAR E -INUL -P%PASS% "%PATH%\%NAME%" "%TMP%"

    std::string line;

    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        std::cerr << line << std::endl;

    c.wait();
#endif


    std::string str = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./ ~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
    //std::string str = "`1234567890";

    int size = str.size();
    std::vector<int> vec;
    int n = 3;
    int index;

    vec.resize(3);

    std::string output;
    output.resize(n);

    
    while(true)
    {        
        for( int i = 0; i < vec.size(); i++ )
            output[i] = str[vec[i]];
        std::cout << output << "\n";

        index = 0;
        while(true)
        {
            vec[index]++;
            if( vec[index] >= size )
            {
                if( index == n-1 )
                    break;
                vec[index] = 0;
                vec[index + 1]++;
                index++;
            }
            if( vec[index] < size )
                break;
        }

        if( vec[n - 1] >= size )
            break;
    }
    



    return true;
}
