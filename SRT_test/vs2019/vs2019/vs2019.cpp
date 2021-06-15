#include <iostream>

#include "srt_client.h"
#include "srt_server.h"


/*
    備忘
    #ifdef SEND_DATA
    _setmode( _fileno(stdin), O_BINARY );
    #elif defined(RECV_DATA)
    _setmode( _fileno(stdout), O_BINARY );
    #endif
    做這個的範例
    讓stdout, stdin可以用binary的方式操作
    可以 ffmpeg ... | srt_test  串接不同的程式

*/


int main()
{
    srt_client_test();
    //srt_server_test();

    system("PAUSE");
    return 0;
}

