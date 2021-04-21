#include "tool.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#elif defined(UNIX)
#include <arpa/inet.h>
#endif

#include <stdio.h>



void transform_test()
{
    // 參考 http://haoyuanliu.github.io/2017/01/15/%E5%9C%B0%E5%9D%80%E8%BD%AC%E6%8D%A2%E5%87%BD%E6%95%B0inet-addr-inet-aton-inet-ntoa-%E5%92%8Cinet-ntop-inet-pton/

    char lo[] = "127.0.0.1";
    struct in_addr netAddr;

    netAddr.s_addr = inet_addr(lo);
    printf("NetIP: 0x%x\n", netAddr.s_addr);

    char* strAddr = inet_ntoa(netAddr);
    printf("StrIP: %s\n", strAddr);

#ifdef UNIX
    inet_aton(strAddr, &netAddr);
    printf("NetIP: 0x%x\n", netAddr.s_addr);
#endif


    struct in_addr addr;
    if ( inet_pton(AF_INET, "127.0.0.1", &addr.s_addr) == 1)
        printf("NetIP: %x\n", addr.s_addr);
    char str[20];
    if ( inet_ntop(AF_INET, &addr.s_addr, str, sizeof str))
        printf("StrIP: %s\n", str);
}
