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


/*
    理論上應該可以,但沒有找到很簡單的作法.   1. get MTU
    會自動切段                             2. TCP如何知道一次可以傳多少資料?
 */


#endif
