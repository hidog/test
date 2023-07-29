#ifndef FILE_H
#define FILE_H

#include <string>
#include <boost/filesystem.hpp>

// need add 7z to environment variable.
void    unzip_all_files();
void    unzip_file( boost::filesystem::path filepath );

void    get_all_filelist( const std::string& path );
void    print_all_extension();

#endif
