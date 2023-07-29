#ifndef B_PROCESS_H
#define B_PROCESS_H

#include <string>
#include <boost/filesystem.hpp>

bool    crack_zip( const int pass_len );
bool    unzip( std::string& password, const std::string& tmp_path );
bool    is_unzip_success( const boost::filesystem::path& dir );

// need add environment variable and run with release build.
void    unzip_main();
void    control_unzip( const int thr_id );


#endif