#include "tool_2.h"

#include <netdb.h>
#include <arpa/inet.h>  // 解決 inet_ntop 的 warning

#include <stdio.h>
#include <string.h>



#ifndef NULL
#define NULL 0
#endif



void test_getaddrinfo()
{
    char ip_str[INET6_ADDRSTRLEN];

    int ret;
    struct addrinfo hints, *servinfo, *ptr;

    memset( &hints, 0, sizeof hints );

    hints.ai_family     =   AF_UNSPEC;
    hints.ai_socktype   =   SOCK_DGRAM;
    hints.ai_flags      =   AI_PASSIVE;

    ret = getaddrinfo( NULL, "12345", &hints, &servinfo );
    if( ret != 0)
    {
        printf(  "getaddrinfo: %s\n", gai_strerror(ret) );
        return;
    }

    // 用迴圈來找出全部的結果，並 bind 到首先找到能 bind 的
    for( ptr = servinfo; ptr != NULL; ptr = ptr->ai_next )
    {
        void *addr;
        char *ipver;

        // 取得本身位址的指標，
        // 在 IPv4 與 IPv6 中的欄位不同：
        if( ptr->ai_family == AF_INET) // IPv4
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else // IPv6
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop( ptr->ai_family, addr, ip_str, sizeof ip_str);
        printf( "%s: %s\n", ipver, ip_str );
    }

}
