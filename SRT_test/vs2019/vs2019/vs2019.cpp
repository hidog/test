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
    /*
        測試延遲結果
        * 剛開始的時候會延遲, 但整體來講比過去穩定非常多, 久久才會遇到一次大延遲, 
          並且大延遲的數據也比過去低. (過去延遲高很多)
        * 觀測到的延遲約250ms  (delay = 120ms)
        * 理論上應該要多開一個socket負責回傳,但目的只是簡單測試,就算了
        * 需要 20次round sleep一次, 頻率下降會直接掛掉 
        * 看起來延遲還是無法低於10frame. 
        * 用ffmpeg, 40M再次測試
        * 執行一段時間後(約一小時吧),程序死當,不知道是否為sleep頻率過低造成. 但sleep過高會造成流量不足
    */
    //srt_client_test();
    //srt_server_test();

    system("PAUSE");
    return 0;
}

