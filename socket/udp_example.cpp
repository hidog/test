#include "udp_example.h"
#include <WinSock2.h>
#include <stdio.h>




void udp_hello_client()
{
    // windows need init
    WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup( socket_version, &wsa_data ) != 0 )
    {
		printf("init error\n");
        return;
    }

    // create socket
    SOCKET client_skt = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(12345);
    remote_addr.sin_addr.S_un.S_addr = inet_addr("36.226.252.251");
    int remote_len = sizeof(remote_addr);
    
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

    closesocket( client_skt );
    WSACleanup();
}





void udp_hello_server()
{
    // windows need init
	WORD socket_version = MAKEWORD(2,2);
    WSADATA wsa_data; 
    if( WSAStartup(socket_version,&wsa_data) != 0 )
    {
		printf("init error\n");
        return;
    }

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