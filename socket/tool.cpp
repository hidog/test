#include "tool.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#elif defined(UNIX) || defined(MACOS)
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>      // use for get host by name.
#include <ifaddrs.h>    // use for get local ip.
#include <net/if.h>      // use for get local ip. use ifreq
#include <sys/ioctl.h>
#endif

#include <stdio.h>
#include <iostream>
#include <string.h>


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
#define SOCKET_ERROR -1
#else
typedef int socklen_t;
#endif



#ifdef _WIN32
#include <iphlpapi.h>   //For SendARP.  查詢mac address用
#endif

#ifdef _WIN32
//#pragma comment ( lib, "Ws2_32.lib" )
//#pragma comment ( lib, "Mswsock.lib")
//#pragma comment ( lib, "AdvApi32.lib")
#pragma comment ( lib, "iphlpapi.lib")		// for iphlpapi. 例如查詢mac address會用到.
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
        windows跟linux用的方式不同,不能混用.
    */
#ifdef _WIN32
    int timeout = 0;
#else
    timeval timeout;
#endif
    socklen_t timeout_len = sizeof timeout;
    
#ifdef _WIN32
    res = getsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, &timeout_len );
#else
    res = getsockopt( skt, SOL_SOCKET, SO_SNDTIMEO, &timeout, &timeout_len );
#endif
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
#ifdef _WIN32
    printf( "timeout = %d\n", timeout );
#elif defined(UNIX)
    printf("timeout = %ld s, %ld us\n", timeout.tv_sec, timeout.tv_usec );
#elif defined(MACOS)
    printf("timeout = %ld s, %d us\n", timeout.tv_sec, timeout.tv_usec );
#endif

    
#ifdef _WIN32
    timeout = 1000; // 1s
#else
    timeout.tv_sec = 3;
    timeout.tv_usec = 12000; // ubuntu有精確度問題,會自動變成ms.  (最小值4000倍數)
#endif 
    
    
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
#ifdef _WIN32
    printf( "timeout = %d\n", timeout );
#elif defined(UNIX)
    printf("timeout = %ld s, %ld us\n", timeout.tv_sec, timeout.tv_usec );
#elif defined(MACOS)
    printf("timeout = %ld s, %d us\n", timeout.tv_sec, timeout.tv_usec );
#endif




    /* 
        SO_RCVBUF
    */
    int recvbuf = 0;
    socklen_t recvbuf_len = sizeof recvbuf;
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



void get_domain_name()
{
#ifdef _WIN32
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    // http://www.cs.tau.ac.il/~eddiea/samples/Non-Blocking/tcp-nonblocking-client.c.html
    // http://haoyuanliu.github.io/2017/01/16/%E5%9C%B0%E5%9D%80%E6%9F%A5%E8%AF%A2%E5%87%BD%E6%95%B0gethostbyname-%E5%92%8Cgetaddrinfo/
    hostent *host_name;
    host_name = gethostbyname("www.google.com");
    if( host_name != NULL )
    {
        // 用host name的作法.
        sockaddr_in addr;
        addr.sin_family = AF_INET;        
        //addr.sin_port = htons(1234);        
        addr.sin_addr = *( (in_addr *)host_name->h_addr );        

        // 把網址轉換成ip address的範例
        char* str = inet_ntoa(addr.sin_addr);        
        printf("ip address: %s\n", str );        
    }    

#ifdef _WIN32
    WSACleanup();
#endif

}





// 搜尋了一下,看起來有別的方式可以取得local ip
// http://www.samirchen.com/get-client-server-ip-port/
// getsockname  getpeername   TCP連上線後可以用這個來取得ip address
// https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
// getaddrinfo
void get_local_ip()
{
#ifdef _WIN32
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
        printf("init error\n");
        return;
    }
#endif

    char str[80];
    int ret;    

    hostent	*phe = NULL; 
    
    ret	= gethostname( str, sizeof(str) );    
    printf( "host name = %s\n", str );    

    phe	= gethostbyname(str);    
    if( phe != NULL )    
    {
        // 0 抓到virtualbox的ip. 1 抓到實體網卡的ip. 有空再研究怎麼過濾了.
        // ubuntu底下會抓到127.0.1.1, 搜尋了一下, 用getsockname等會比較好...
        in_addr sin = *( (in_addr *)phe->h_addr_list[0] );
        char *ip = inet_ntoa(sin);
        printf("local ip = %s\n", ip);
    }
    
#ifdef _WIN32
    WSACleanup();
#endif
}









// http://wiki.lang.idv.tw/doku.php/program/c/linux%E7%B7%A8%E7%A8%8B%E7%8D%B2%E5%8F%96%E6%9C%AC%E6%A9%9Fip%E5%9C%B0%E5%9D%80
void get_local_ip_2()
{
// 不確定是否能用在windows平台,就不做確認了.
#if defined(UNIX) || defined(MACOS)
    //ifaddrs* ifAddrStruct = NULL;
    ifaddrs* ifa = NULL;
    //void* tmpAddrPtr = NULL;
    void *ptr = NULL;
 
    getifaddrs(&ifa);
 
    while( ifa != NULL ) 
    {
        if( ifa->ifa_addr->sa_family == AF_INET ) 
        { 
            // check it is IP4
            // is a valid IP4 Address
            ptr = &((sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char address[INET_ADDRSTRLEN];
            inet_ntop( AF_INET, ptr, address, INET_ADDRSTRLEN );
            printf( "%s IP Address %s\n", ifa->ifa_name, address );             
        } 
        else if( ifa->ifa_addr->sa_family == AF_INET6 ) 
        { 
            // check it is IP6
            // is a valid IP6 Address
            ptr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char address[INET6_ADDRSTRLEN];
            inet_ntop( AF_INET6, ptr, address, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, address); 
        } 
        
        ifa = ifa->ifa_next;
    }
#else
    printf("this function use for ubuntu.\n");
#endif
}




void get_local_ip_3()
{
#if defined(UNIX) || defined(MACOS)
    int sock_get_ip;  
    char ipaddr[50];  
 
    sockaddr_in *sin;  
    ifreq ifr_ip;     
 
    sock_get_ip = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    memset( &ifr_ip, 0, sizeof(ifr_ip) );
#ifdef UNIX
    strncpy( ifr_ip.ifr_name, "enp6s0", sizeof(ifr_ip.ifr_name) - 1);  // 範例用eth0, 失敗. 改用 enp6s0, 成功. 如何取得網卡的名稱呢?
#else
    strncpy( ifr_ip.ifr_name, "en0", sizeof(ifr_ip.ifr_name) - 1); // mac用 en0 才成功.
#endif
 
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip ) < 0 )     
    {     
        printf("get_local_ip_3 error\n");
        return;
    }   
        
    sin = (sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy( ipaddr, inet_ntoa(sin->sin_addr) );
 
    printf("local ip : %s \n", ipaddr );
    close( sock_get_ip );  
#else
    printf("use for ubuntu.\n");
#endif
}



void	get_mac_address()
{
#ifdef _WIN32
    //const int	mac_len	=	12;
    unsigned char mac[12];

    int     i;
    DWORD	ret;
    IPAddr	src_ip;
    ULONG	mac_addr[2];
    ULONG	phy_addr_len	=	6; 
    BYTE	*b_mac_addr		=	NULL;

    // 指向目標網卡的ip address.
    in_addr sin;
    sin.S_un.S_addr = inet_addr( "192.168.1.102" ); //inet_addr( "192.168.1.2" );


    //
    src_ip	=	0;
    ret		=	SendARP( (IPAddr)sin.S_un.S_addr , src_ip , mac_addr , &phy_addr_len);

    //
    if( phy_addr_len )
    {
        b_mac_addr	=	(BYTE*)&mac_addr;
        for ( i = 0; i < (int)phy_addr_len; i++ )
        {
            mac[i]	=	(char)b_mac_addr[i];
            printf("%X ", mac[i] );
        }        
        printf("\n");
    }
#elif defined(UNIX)
    int sock_mac;  
 
    ifreq ifr_mac;  
    char mac_addr[30];     
 
    sock_mac = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );  
    if( sock_mac == -1)  
    {  
        printf("create socket falise...mac\n");  
        return;  
    }  
 
    memset( &ifr_mac, 0, sizeof(ifr_mac) );     
    strncpy( ifr_mac.ifr_name, "enp6s0", sizeof(ifr_mac.ifr_name)-1 );   // 範例程式用eth0. 改成enp6s0   
 
    if( ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac) < 0 )
    {  
        printf("mac ioctl error\n");  
        return;
    }  
 
    sprintf( mac_addr,"%02x %02x %02x %02x %02x %02x",  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],  
             (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5] );
 
    printf("local mac : %s \n", mac_addr );
 
    close( sock_mac );  
#else
    // mac下, 找不到 SIOCGIFHWADDR 這個定義
    // 有空再研究了.
    printf("not implement\n");
#endif
}
