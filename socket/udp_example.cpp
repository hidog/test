#include "udp_example.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <WinSock2.h>
#elif defined(UNIX)
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <arpa/inet.h>  // 抽空研究這些header file
#endif



#ifdef UNIX
typedef int SOCKET;
#endif



void udp_hello_client()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
		printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(12345);
    
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr("36.226.252.251");
    int remote_len = sizeof(remote_addr); 
#elif defined(UNIX)
    remote_addr.sin_addr.s_addr = inet_addr( "36.226.252.251" );
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 有空研究一下這些函數的差別,是否有多重寫法
    socklen_t remote_len = sizeof(remote_addr);
#endif
    
    // send data
    char send_data[100] = "hello, server. this is client\n";
    int ret;
    ret = sendto( client_skt, send_data, strlen(send_data), 0, (sockaddr *)&remote_addr, remote_len );
    printf( "client send, ret = %d\n", ret );

    // receive back
    char recv_data[100];
    ret = recvfrom( client_skt, recv_data, 100, 0, (sockaddr *)&remote_addr, &remote_len );
    if( ret > 0 )
        printf( "receive message from server. ret = %d, data = %s\n", ret, recv_data );
    else    
        printf( "ret = %d <= 0. error\n", ret );

#ifdef _WIN32
    closesocket( client_skt );
    WSACleanup();
#elif defined(UNIX)
    close(client_skt);
#endif
}




#if 0
void udp_hello_server()
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

    // bind server socket
    SOCKET server_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in local_addr;
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(12345);
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
    int local_len = sizeof(local_addr);
    
    if( bind(server_skt, (sockaddr*)&local_addr, local_len ) == SOCKET_ERROR )
    {
        printf("bind error !\n");
        closesocket(server_skt);
        WSACleanup();
        return;
    }

    printf( "bind success. wait for client data...\n" );

    // receive remote data
    sockaddr_in remote_addr;
    int remote_len = sizeof(remote_addr);

    char recv_data[100] = {0};
    int ret;
    ret = recvfrom( server_skt, recv_data, sizeof recv_data, 0, (sockaddr*)&remote_addr, &remote_len );

    if( ret > 0 )    
        printf( "recv data. ret = %d, recv_data = %s\n", ret, recv_data );    
    else
    {
        printf( "ret = %d, error\n", ret );
        closesocket( server_skt );
        WSACleanup();
        return;
    }

    // send back
    char send_data[100] = "hello, this is server.\n";
    ret = sendto( server_skt, send_data, strlen(send_data), 0, (sockaddr*)&remote_addr, remote_len );
    printf( "send back. ret = %d\n", ret );

    closesocket(server_skt);
    WSACleanup();
}
#endif
