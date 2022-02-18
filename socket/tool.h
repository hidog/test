#ifndef TOOL_H
#define TOOL_H


/*
https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedrawsocket11d.html
有時間研究這段code
*/



void transform_test();
void error_handle_test();
void sockopt_test();

void get_domain_name();
void get_local_ip();
void get_mac_address();

void get_local_ip_2();  // 這個會列舉全部的資訊
void get_local_ip_3();

/*
    get local ip不是很可靠,在ubuntu下會抓到127.0.1.1, 有需要再研究有沒有更好的做法
    改用 getpeername, getsockname, 已連線的話可以抓到對外的ip
    get mac address沒測mac環境,有需要再測了
    
    get peer/sock name 參考程式碼 tcp_server_non_blocking , tcp_nonblock.h
    裡面的 tcp_show_ip_address (沒處理error code)
    
*/



/*
    理論上應該可以,但沒有找到很簡單的作法.   1. get MTU
    會自動切段                          2. TCP如何知道一次可以傳多少資料?
 
 ifreq  裡面有MTU參數,有機會再研究吧
 */


#endif
