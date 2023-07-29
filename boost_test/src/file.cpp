#include "file.h"

#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <sstream>
#include <cassert>
#include <list>

#include <boost/filesystem.hpp>
#include <boost/process.hpp>




void    unzip_file( boost::filesystem::path filepath )
{
    boost::process::ipstream    pipe_stream;

    std::ostringstream  os;
    os << "7z.exe x " << filepath << " -o\"" << "J:\\output\\" << filepath.stem().string() << "\"";
    std::cout << os.str() << "\n";

    boost::process::child c( os.str(), boost::process::std_out > pipe_stream );
    //UNRAR E -INUL -P%PASS% "%PATH%\%NAME%" "%TMP%"

    std::string     line;
    while( pipe_stream && std::getline(pipe_stream, line) && !line.empty() )
        std::cerr << line << std::endl;
    c.wait();
}




void    unzip_all_files()
{
    boost::filesystem::path     path("J:\\[Nipponsei] archive v20150127\\");
    //boost::filesystem::path     path("D:\\");

    std::string     extension;
    int     count   =   0;

    try
    {
        if( boost::filesystem::exists(path) == true )
        {
            if( boost::filesystem::is_regular_file(path) )
                std::cout << path << " size is " << boost::filesystem::file_size(path) << '\n';
            else if( boost::filesystem::is_directory(path) == true )
            {
                std::cout << path << " is a directory containing:\n";

                /*std::copy( boost::filesystem::directory_iterator(path), 
                    boost::filesystem::directory_iterator(),
                    std::ostream_iterator<boost::filesystem::directory_entry>( std::cout, "\n") );*/
                for( auto itr : boost::filesystem::directory_iterator(path) )
                {
                    //std::cout << itr.path() << "\n";
                    extension   =   boost::filesystem::extension(itr.path());
                    //if( extension != ".zip" )
                    std::cout << itr.path() << " " << extension << "\n";
                    count++;
                    boost::filesystem::path filepath(itr.path());
                    if( extension == ".zip" || extension == ".rar" )
                        unzip_file( filepath );
                }
            }
            else
                std::cout << path << " exists, but is neither a regular file nor a directory\n";
        }
        else
            std::cout << path << " does not exist\n";
    }
    catch( const boost::filesystem::filesystem_error& ex )
    {
        std::cout << ex.what() << '\n';
    }

    std::cout << "count = " << count << "\n";
}



std::list<std::string>  extension_list;
std::list<std::string>  remove_list;


void    get_all_filelist( const std::string& path )
{
    boost::filesystem::path     dir_path(path);

    std::string     extension;

    try
    {
        if( boost::filesystem::exists(dir_path) == true )
        {
            if( boost::filesystem::is_directory(dir_path) == true )
            {
                for( auto itr : boost::filesystem::directory_iterator(dir_path) )
                {
                    if( boost::filesystem::is_directory(itr.path()) == true )   
                        get_all_filelist(itr.path().string());
                    else
                    {
                        extension   =   boost::filesystem::extension(itr.path());
                        if( std::find( extension_list.begin(), extension_list.end(), extension ) == std::end(extension_list) )
                            extension_list.push_back(extension);
                        if( extension == "" )
                            //remove_list.emplace_back( itr.path().string() );
                            std::cout << itr.path() << "\n\n";
                    }
                }
            }
            else
                std::cout << path << " exists, but is neither a regular file nor a directory\n";
        }
        else
            std::cout << dir_path << " does not exist\n";
    }
    catch( const boost::filesystem::filesystem_error& ex )
    {
        std::cout << ex.what() << '\n';
    }
}



void    print_all_extension()
{
    get_all_filelist("J:\\output");

    for( auto itr : extension_list )
        std::cout << itr << "\n";

    std::cout << "\n";

    for( auto itr : remove_list )
    {
        std::cout << "remove : " << itr << "\n\n";
        boost::filesystem::path     file(itr);
        boost::filesystem::remove(file);
    }
}



