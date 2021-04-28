#include "tool.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#elif defined(UNIX) || defined(MACOS)
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <stdio.h>


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
#define SOCKET_ERROR -1
#endif



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




void error_handle_test()
{
#ifdef UNIX
    // 這邊印出0, 下面會印出22.
    printf("errno = %d\n", errno);
#endif
    

#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // bind server socket
    SOCKET server_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );

    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(12345);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#elif defined(UNIX) || defined(MACOS)
    local_addr.sin_addr.s_addr = INADDR_ANY; 
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if( bind(server_skt, (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
        printf("bind error !\n");
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#else
        close(server_skt);
#endif
        return;
    }


    // 這邊會跳error
    if( bind(server_skt, (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
#ifdef _WIN32    
        printf("bind error ! %d\n", WSAGetLastError() );
#else
        printf("bind error ! %d\n", errno );
#endif
        
        
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#else
        close(server_skt);
#endif
        return;
    }
}




/*
    windows ref: https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-setsockopt
                 https://docs.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-getsockopt
*/
void sockopt_test()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    SOCKET skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    int res;

    /* 
        SO_SNDTIMEO
    */
    int timeout = 0;
    int timeout_len = sizeof timeout;
    res = getsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, &timeout_len );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }
    printf( "timeout = %d\n", timeout );

    timeout = 1000; // 1s
    res = setsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof timeout );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }

    // reget sent timeout
    res = getsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, &timeout_len );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }
    printf( "timeout = %d\n", timeout );





    /* 
        SO_RCVBUF
    */
    int recvbuf = 0;
    int recvbuf_len = sizeof recvbuf;
    res = getsockopt( skt, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuf, &recvbuf_len );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }
    printf( "recvbuf = %d\n", recvbuf );

    recvbuf = 30; // 1s
    res = setsockopt( skt, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuf, sizeof recvbuf_len );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }

    // reget sent timeout
    res = getsockopt( skt, SOL_SOCKET, SO_RCVBUF, (char*)&recvbuf, &recvbuf_len );
    if( res == SOCKET_ERROR )
    {
#ifdef _WIN32
        int err_code = WSAGetLastError();
#else
        int err_code = errno;
#endif
        printf("error code = %d\n", err_code );
        return;
    }
    printf( "recvbuf = %d\n", recvbuf );




#ifdef _WIN32
    closesocket(skt);
#else
    close(skt);
#endif
}
