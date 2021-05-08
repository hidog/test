#ifndef TCP_EXAMPLE_H
#define TCP_EXAMPLE_H



void tcp_hello_server( int port );
void tcp_hello_client( const char *ip, int  port );


/*
	1. hello
	2. tcp non-block
	   a. 是否有機會資料傳一半
	   b. 一次傳送的封包上限
	   c. 斷線測試
	3. 上述行為在測一次
    4. connect timeout, recv timeout等等. 測試結果發現這邊很多不明的點,有研究的必要
    5. 故意設計會報錯的範例,測試error code. 
*/

#endif
