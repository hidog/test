#include "process.h"

#include <boost/process.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <sstream>
#include <boost/thread.hpp>



using namespace boost::process;
using namespace boost::filesystem;


const std::string dictionary = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./ ~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
const int dict_size = dictionary.size();

std::string filename;
std::string filepath;
int thread_count = 1;

std::string true_password;
bool is_finish = false;




bool    is_unzip_success( const path& dir )
{
  std::vector<path> list;
  copy( directory_iterator(dir), directory_iterator(), back_inserter( list ) );
  if( list.size() > 0 )
      return true;
  else
      return false;
}



bool    unzip( std::string& password, const std::string& tmp_path )
{
    ipstream pipe_stream;

    std::ostringstream os;
    os << "UnRAR.exe x -inul -p\"" << password << "\" \"" << filepath << "\\" << filename << "\" \"" << tmp_path << "\"";
    //std::cout << os.str() << "\n";

    child c( os.str(), std_out > pipe_stream );
    //UNRAR E -INUL -P%PASS% "%PATH%\%NAME%" "%TMP%"

    std::string line;
    while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        std::cerr << line << std::endl;
    c.wait();

    return true;
}





bool    crack_zip( const int pass_len )
{
    std::vector<int> vec;
    int index;

    vec.resize(pass_len);
    std::string password;
    password.resize(pass_len);

    std::ostringstream os;
    os << filepath << "\\" << "tmp" << pass_len;
    std::string tmp_path = os.str();
    if( false == create_directory( tmp_path ) )
        std::cout << "\n\n\n !!! error !!!\n\n\n";
    
    int count = 0;

    while( is_finish == false )
    {        
        if( count % 10000 == 0 )
            std::cout << "pass_len = " << pass_len << " run " << count << " times\n";
        count++;

        for( int i = 0; i < vec.size(); i++ )
            password[i] = dictionary[vec[i]];
        //std::cout << password << "\n";

        unzip( password, tmp_path );
        if( is_unzip_success( tmp_path ) == true )
        {
            is_finish = true;
            true_password = password;
            break;
        }

        index = 0;
        while(true)
        {
            vec[index]++;
            if( vec[index] >= dict_size )
            {
                if( index == pass_len - 1 )
                    break;
                vec[index] = 0;
                vec[index + 1]++;
                index++;
            }
            if( vec[index] < dict_size )
                break;
        }

        if( vec[pass_len - 1] >= dict_size )
            break;
    }    

    std::cout << "password = " << password << "\n";

    return true;
}




void    control_unzip( const int thr_id )
{
    for( int pass_len = thr_id; pass_len <= 30; pass_len += thread_count )    
        crack_zip( pass_len );    
}



void    unzip_main()
{
    is_finish = false;

    std::cout << "input filename : ";
    std::cin >> filename;

    std::cout << "input filepath : ";
    std::cin >> filepath;

    std::cout << "input thread count : ";
    std::cin >> thread_count;

    boost::thread **thr = nullptr;
    thr = new boost::thread*[thread_count];
    for( int thr_id = 0; thr_id < thread_count; thr_id++ )
        thr[thr_id] = new boost::thread( &control_unzip, thr_id + 1 );

    for( int i = 0; i < thread_count; i++ )
        thr[i]->join();
    for( int i = 0; i < thread_count; i++ )    
        delete thr[i];
    delete [] thr;

    std::cout << "true_password = " << true_password << "\n\n\n";
}
