#include "file.h"
#include <fstream>
#include <stdio.h>

uint64_t get_file_size()
{
   uint64_t length = -1;

   std::ifstream file_fstream( "D:\\code\\test.mkv", std::ifstream::binary);
   if (file_fstream.is_open())
   {
      // get length of file:
      file_fstream.seekg(0, file_fstream.end);
      length = file_fstream.tellg(); 
      file_fstream.seekg(0, file_fstream.beg); // 如果需要讀取檔案,則需設置回起點
      file_fstream.close();
    }
    else
        printf( "load fail\n" );

    return length;       
}
