#include "b_process.h"

#include <boost/process.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>
#include <boost/filesystem.hpp>
#include <vector>
#include <sstream>
#include <boost/thread.hpp>
#include <fstream>



using namespace boost::process;
using namespace boost::filesystem;


//const std::string dictionary = "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./ ~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
//const std::string dictionary = "1234567890-qwertyuiopasdfghjklzxcvbnm.@QWERTYUIOPASDFGHJKL:ZXCVBNM";
//const std::string dictionary = "qQwWeErRtTyYuUiIoOpPaAsSdDfFgGhHjJkKlLzZxXcCvVbBnNmM1234567890-.@:/\\";
const std::string dictionary = "012345";
const int dict_size = dictionary.size();

std::string filename;
std::string filepath;
int thread_count = 1;

std::string true_password;
bool is_finish = false;

boost::mutex io_mutex;



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




bool    unzip_7z( std::string& password, const std::string& tmp_path )
{
    bool result = true;
    ipstream pipe_stream;

    std::ostringstream os;
    os << "C:\\Program Files\\7-Zip\\7z.exe x " << filename << " -p\"" << password << "\" -o\"" << tmp_path << "\"";
    //std::cout << os.str() << "\n";

    try {
        child c( os.str(), std_out > pipe_stream );
        // https://hooo.medium.com/%E5%B7%A5%E4%BD%9C%E7%AD%86%E8%A8%98-%E9%80%8F%E9%81%8E7zip%E6%8C%87%E4%BB%A4%E5%B0%8D%E6%AA%94%E6%A1%88%E9%80%B2%E8%A1%8C%E5%A3%93%E7%B8%AE%E8%88%87%E8%A7%A3%E5%A3%93%E7%B8%AE-861da198932
        // 7z x D:\test.rar -p"1a234" -o"D:\test"

        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        {
            //std::cerr << line << std::endl;
            if( line.find("Errors") != std::string::npos )
            {
                result = false;
                break;
            }
        }
        c.wait();    
    }
    catch(std::exception e)
    {
        std::cout << os.str();
        std::cout << e.what();
    }


    return result;
}





bool    crack_zip( const int pass_len )
{
    bool result;
    std::vector<int> vec;
    int index;

    vec.resize(pass_len);
    std::string password;
    password.resize(pass_len);

    std::ostringstream os;
    os << filepath << "\\" << "tmp" << pass_len;
    std::string tmp_path = os.str();
    //if( false == create_directory( tmp_path ) )
      //  std::cout << "\n\n\n !!! error !!!\n\n\n";
    
    int count = 0;

    while( is_finish == false )
    {        
        if( false == create_directory( tmp_path ) )
            std::cout << "\n\n\n !!! error !!!\n\n\n";

        for( int i = 0; i < vec.size(); i++ )
            password[vec.size() - i - 1] = dictionary[vec[i]];
        //std::cout << password << std::endl;

        if( password == "0" )
            std::cout << "Test";

        if( count % 10000 == 0 )
            std::cout << "password = " << password << " pass_len = " << pass_len << " run " << count << " times\n";
        count++;

        //unzip( password, tmp_path );
        result = unzip_7z( password, tmp_path );

        if( result == true )
        {
            is_finish = true;
            true_password = password;
            break;
        }
        else
        {
            try 
            {
                if( false == remove_all( tmp_path ) )
                    std::cout << "\n\n\n !!! remove tmpdir fail !!!\n\n\n";
            }
            catch ( std::exception e )
            {
                std::cout << e.what();
            }
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

    //std::cout << "password = " << password << "\n";

    return result;
}




void    control_unzip( const int thr_id )
{
    for( int pass_len = thr_id; pass_len <= 30; pass_len += thread_count )    
    {
        bool    result  =   crack_zip( pass_len );
        if( result == true )
            break;
    }
}



void    unzip_main()
{
    is_finish = false;

    std::cout << "input filename : ";
    //std::cin >> filename;
    filename = "D:\\test.rar";
    std::cout << "filename = " << filename << std::endl;

    std::cout << "input filepath : ";
    //std::cin >> filepath;
    filepath = "F:\\tmp\\";
    std::cout << "filepath = " << filepath << std::endl;

    std::cout << "input thread count : ";
    //std::cin >> thread_count;
    thread_count = 20;

    boost::thread **thr = nullptr;
    thr = new boost::thread*[thread_count];
    for( int thr_id = 0; thr_id < thread_count; thr_id++ )
        thr[thr_id] = new boost::thread( &control_unzip, thr_id + 1 );

    for( int i = 0; i < thread_count; i++ )
        thr[i]->join();
    for( int i = 0; i < thread_count; i++ )    
        delete thr[i];
    delete [] thr;

    std::cout << "\n\n\ntrue_password = " << true_password << "\n\n\n";

    std::ofstream out("true_password.txt");
    out << "pw len = " << true_password.size() << std::endl;
    out << true_password << std::endl;
    out.close();
}





void    process_example()
{
    // example 1, system info
#ifdef MSVC
    std::cout << "example 1, system info\n\n\n";
    std::ostringstream  os;
    os << "systeminfo";
#else
    std::cout << "example 1, screenfetch\n\n\n";
    std::ostringstream  os;
    os << "screenfetch";
#endif

    try {
        ipstream    pipe_stream;
        child   ch( os.str(), std_out > pipe_stream );
        std::string     line;
        while( pipe_stream && std::getline( pipe_stream, line ) && line.empty() == false )       
            std::cout << line << std::endl;        
        ch.wait();    
    }
    catch( std::exception exception )
    {
        std::cout << os.str();
        std::cout << exception.what();
    }
    os.str(""); // remove data in os.
 
    // example 2, ip config
#ifdef MSVC
    std::cout << "\n\n\nexample 2, ip config\n\n\n";
    os << "ipconfig";
#else
    std::cout << "\n\n\nexample 2, if config\n\n\n";
    os << "ifconfig";
#endif

    try {
        ipstream    pipe_stream;
        child   ch( "ipconfig", std_out > pipe_stream );
        std::string     line;
        while( pipe_stream && std::getline( pipe_stream, line ) && line.empty() == false )       
            std::cout << line << std::endl;        
        ch.wait();    
    }
    catch( std::exception exception )
    {
        std::cout << os.str();
        std::cout << exception.what();
    }
}





void    unrar_main()
{
    is_finish = false;

    std::cout << "input filename : ";
    //std::cin >> filename;
    filename = "D:\\test.rar";
    std::cout << "filename = " << filename << std::endl;

    std::cout << "input filepath : ";
    //std::cin >> filepath;
    filepath = "F:\\tmp\\";
    std::cout << "filepath = " << filepath << std::endl;

    std::cout << "input thread count : ";
    thread_count = 4;
    std::cout << "thread_count = " << thread_count << "\n";
    if( thread_count > dict_size )
    {
        std::cout << "thread_count is larger than dic size = " << dict_size << "\n";
        return;
    }

    int pass_len = 4;
    std::cout << "pass_len = " << pass_len << "\n";

    boost::thread **thr = nullptr;
    thr = new boost::thread*[thread_count];
    for( int thr_id = 0; thr_id < thread_count; thr_id++ )
    {
        thr[thr_id] = new boost::thread( &crack_rar, thr_id, pass_len );
    }

    for( int i = 0; i < thread_count; i++ )
        thr[i]->join();
    for( int i = 0; i < thread_count; i++ )    
        delete thr[i];
    delete [] thr;

    std::cout << "\n\n\ntrue_password = " << true_password << "\n\n\n";

    std::ofstream out("true_password.txt");
    out << "pw len = " << true_password.size() << std::endl;
    out << true_password << std::endl;
    out.close();
}




void    crack_rar( const int thr_id, const int pass_len )
{
    bool result;
    std::vector<int> vec;
    int index;

    vec.resize(pass_len);
    vec[0] = thr_id;
    std::string password;
    password.resize(pass_len);

    std::ostringstream os;
    os << filepath << "\\" << "tmp" << thr_id;
    std::string tmp_path = os.str();
    //if( false == create_directory( tmp_path ) )
      //  std::cout << "\n\n\n !!! error !!!\n\n\n";
    
    int count = 0;

    if( false == create_directory( tmp_path ) )
    {
        std::cout << "\n\n\n !!! error !!!\n\n\n";
        return;
    }

    while( is_finish == false )
    {        
        for( int i = 0; i < vec.size(); i++ )
            password[i] = dictionary[vec[i]];
 
        //io_mutex.lock();
        if( thr_id == 3 )
            std::cout << password << ", count = " << count << "\n";
        //io_mutex.unlock();

        /*if( count % 10 == 0 )
            std::cout << "password = " << password << " pass_len = " << pass_len << " run " << count << " times\n";*/
        count++;

        //unzip( password, tmp_path );
        result = unrar( password, tmp_path );

        if( result == true )
        {
            is_finish = true;
            true_password = password;
            break;
        }

        index = 0;
        vec[index] += thread_count;

        while(true)
        {
            if( vec[index] >= dict_size )
            {
                if( index == pass_len - 1 )
                    break;
                vec[index] -= dict_size;
                vec[index + 1]++;
                index++;
            }
            if( vec[index] < dict_size )
                break;
        }

        if( vec[pass_len - 1] >= dict_size )
            break;
    }    

    io_mutex.lock();
    //std::cout << "thr_id = " << thr_id << ", count = " << count << "\n";
    io_mutex.unlock();

    std::cout << "password = " << password << "\n";
}






bool    unrar( std::string& password, const std::string& tmp_path )
{
    return false;

    bool result = true;
    ipstream pipe_stream;

    std::ostringstream os;
    os << "C:\\Program Files\\7-Zip\\7z.exe x " << filename << " -p\"" << password << "\" -o\"" << tmp_path << "\"";
    //std::cout << os.str() << "\n";

    try {
        child c( os.str(), std_out > pipe_stream );
        // https://hooo.medium.com/%E5%B7%A5%E4%BD%9C%E7%AD%86%E8%A8%98-%E9%80%8F%E9%81%8E7zip%E6%8C%87%E4%BB%A4%E5%B0%8D%E6%AA%94%E6%A1%88%E9%80%B2%E8%A1%8C%E5%A3%93%E7%B8%AE%E8%88%87%E8%A7%A3%E5%A3%93%E7%B8%AE-861da198932
        // 7z x D:\test.rar -p"1a234" -o"D:\test"

        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
        {
            //std::cerr << line << std::endl;
            if( line.find("Errors") != std::string::npos )
            {
                result = false;
                break;
            }
        }
        c.wait();    
    }
    catch(std::exception e)
    {
        std::cout << os.str();
        std::cout << e.what();
    }


    return result;
}

