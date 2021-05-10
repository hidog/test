#ifndef TCP_EXAMPLE_H
#define TCP_EXAMPLE_H



void tcp_hello_server( int port );
void tcp_hello_client( const char *ip, int  port );

void tcp_client_timeout_test();
void tcp_server_timeout_test();

void tcp_size_server( int port );
void tcp_size_client( const char* const ip, int port );  
// const char*     ip[0] = 3 會錯, ip = "abc" 沒問題
// const char const*     效果一樣
// const char* const   ip = "Abc", ip[0] = 3 都會錯.
// char* const    ip[0] = 3 可以,   ip = "abc"會錯


/*
    先呼叫client的connect, server端才listen,也有機會連線成功
*/



/*
    下禮拜優先事項
    1. 研究tcp傳輸上限 (是否有上限?或者跟buffer有關?)
    2. 測試tcp資料傳送一半. (send/recv端都要測試)
*/


/*
	1. hello
	2. tcp non-block
	   a. 是否有機會資料傳一半
	   b. 一次傳送的封包上限
	   c. 斷線測試
	3. 上述行為在測一次
    4. connect timeout, recv timeout等等. 測試結果發現這邊很多不明的點,有研究的必要
    5. 故意設計會報錯的範例,測試error code. 

    6. 測試client先呼叫connect, server才listen.
*/

#endif
