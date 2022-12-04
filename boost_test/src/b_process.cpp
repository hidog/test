#include "process.h"

#include <boost/process.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <sstream>


using namespace boost::process;
using namespace boost::filesystem;

bool ScanDirectory( const path& rPath )
{
  std::cout << " Directory: " << rPath << std::endl;
  std::vector<path> vList;
  copy( directory_iterator(rPath), directory_iterator(), back_inserter( vList ) );
  /*for( auto itr = vList.begin(); itr != vList.end(); ++itr )
  {
    std::cout << "File: " << itr->filename() << "\n";
  }*/

  if( vList.size() > 0 )
      return true;
  else
      return false;
}



bool    unzip( std::string& password )
{
    ipstream pipe_stream;
    //char str[1000];
    //sprintf( str, "J:\\UnRAR.exe x -P%s \"J:\\test.rar\" \"J:\\tmp\"", password.c_str() );

    std::ostringstream os;
    os << "J:\\UnRAR.exe x -inul -p\"" << password << "\" \"J:\\test.rar\" \"J:\\tmp\"";
    std::cout << os.str() << "\n";


    child c( os.str(), std_out > pipe_stream );

    //UNRAR E -INUL -P%PASS% "%PATH%\%NAME%" "%TMP%"

    std::string line;

    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        std::cerr << line << std::endl;

    c.wait();

    return true;
}





bool    crack_zip()
{
    //std::cout << (Scairectory("J:\\test\\tmp") ? "yes" : "no");
    //return true;




    std::string str = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./ ~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
    //std::string str = "`1234567890";

    int size = str.size();
    std::vector<int> vec;
    int n = 3;
    int index;

    vec.resize(n);

    std::string output;
    output.resize(n);

    
    while(true)
    {        
        for( int i = 0; i < vec.size(); i++ )
            output[i] = str[vec[i]];
        //std::cout << output << "\n";

        unzip( output );
        if( ScanDirectory( "J:\\tmp" ) == true )
            break;


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
    

    std::cout << "password = " << output << "\n";

    return true;
}
