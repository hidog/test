#include <iostream>

#include "srt_client.h"
#include "srt_server.h"

#include "ffmpeg_server.h"
#include "ffmpeg_client.h"


/*
    _setmode( _fileno(stdin), O_BINARY );
    _setmode( _fileno(stdout), O_BINARY );
    讓stdout, stdin可以用binary的方式操作
    可以 ffmpeg ... | srt_test  串接不同的程式
*/


int main( int argc, char* argv[] )
{
    /*
        測試延遲結果
        * 剛開始的時候會延遲, 但整體來講比過去穩定非常多, 久久才會遇到一次大延遲, 
          並且大延遲的數據也比過去低. (過去延遲高很多)
        * 觀測到的延遲約250ms  (delay = 120ms)
        * 理論上應該要多開一個socket負責回傳,但目的只是簡單測試,就算了
        * 需要 20次round sleep一次, 頻率下降會直接掛掉 
        * 看起來延遲還是無法低於10frame. 
        * 執行一段時間後(約一小時吧),程序死當,不知道是否為sleep頻率過低造成. 但sleep過高會造成流量不足

        * 用ffmpeg, 40M再次測試, 確認一下實用性
    */
    //srt_client_test();
    //srt_server_test();

    std::string ip, port;

    if( argc == 2 )
    {
        port = std::string(argv[1]);
        ffmpeg_server(port);
    }
    else if( argc == 3 )
    {
        ip = std::string(argv[1]);
        port = std::string(argv[2]);
        ffmpeg_client(ip,port);
    }

    return 0;
}




/*
拿來study各種lock的用法

void output_to_stdout()
{
    while(true)
    {
        if( g_list.empty() == true )
            Sleep(1);
        else
        {
            std::unique_lock<std::mutex> lck(g_mtx,std::defer_lock);
            lck.lock();
            char *ptr = g_list.front();
            g_list.pop_front();
            lck.unlock();

            fwrite( ptr, 1316, 1, stdout );
            delete [] ptr;
        }
    }
}
*/

