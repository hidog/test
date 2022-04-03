#ifndef TCP_EXAMPLE_H
#define TCP_EXAMPLE_H



void tcp_hello_server( int port );
void tcp_hello_client( const char *ip, int  port );

void tcp_client_timeout_test();
void tcp_server_timeout_test();

/*
    buffer_size = 300, recv端發生240
    加上sleep後有改善,但仍有機會發生
    buffer開9999999,一下就報錯
    網頁搜尋到的結果,一般不超過64k 65536
    可以參考MTU   通常是14xx, 某些環境可能會在500以下.
    這是建議值, TCP的話會自己切
    看起來行為會隨著網路環境不同有所改變. (ubuntu/mac測到不同的結果,但結論是都有機會沒傳完)
*/
void tcp_size_server( int port );
void tcp_size_client( const char* const ip, int port );  
// const char*     ip[0] = 3 會錯, ip = "abc" 沒問題
// const char const*     效果一樣
// const char* const   ip = "Abc", ip[0] = 3 都會錯.
// char* const    ip[0] = 3 可以,   ip = "abc"會錯


/*
    先呼叫client的connect, server端才listen,也有機會連線成功
*/



#endif
