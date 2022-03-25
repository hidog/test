#include "p2p_example.h"

#include <stdio.h>

#ifdef _WIN32
#include <WinSock2.h>
#else
#error undefined
#endif


#if defined(UNIX) || defined(MACOS)
typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#elif defined(_WIN32)
#define bzero(ptr,size)     memset( (ptr), 0, (size) )
typedef int socklen_t;
typedef int ssize_t; // 查了一下windows沒找到ssize_t這個定義,在研究一下. 看討論目前非標準定義
#endif


constexpr int p2p_server_port_1 = 1234;
constexpr int p2p_client_port_1 = 1235;

constexpr int p2p_server_port_2 = 2345;






void p2p_bind_client_socket(int skt)
{
    // 用底下的bind可以做到指定client port的效果,但不設定也會自動設定port.
    sockaddr_in local_addr;
    bzero(&local_addr, sizeof local_addr);  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(p2p_client_port_1);  // local port指定24256

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#else
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if (bind(skt, (sockaddr*)&local_addr, local_len) == SOCKET_ERROR)
    {
        printf("bind error !\n");
        return;
    }
}





void p2p_server_1()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsa_data;
    if (WSAStartup(socket_version, &wsa_data) != 0)
    {
        printf("init error\n");
        return;
    }
#endif

    // bind server socket
    SOCKET server_skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in local_addr;
    bzero(&local_addr, sizeof local_addr);  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(p2p_server_port_1);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#elif defined(UNIX) || defined(MACOS)
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if (bind(server_skt, (sockaddr*)&local_addr, local_len) == SOCKET_ERROR)
    {
        printf("bind error !\n");
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }

    printf("bind success. wait for client data...\n");

    // receive remote data
    sockaddr_in remote_addr;
    bzero(&remote_addr, sizeof remote_addr);
    socklen_t remote_len = sizeof(remote_addr);

    // 
    char recv_data[30] = { 0 };
    ssize_t ret;
    ret = recvfrom(server_skt, recv_data, sizeof recv_data, 0, (sockaddr*)&remote_addr, &remote_len);
    recv_data[ret] = '\0';

    if (ret > 0)
    {
        printf("recv from ip = %s, port = %d\n", inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
        printf("recv data. ret = %ld, recv_data = %s\n", ret, recv_data);
    }
    else
    {
        printf("ret = %ld, error\n", ret);
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }

#ifdef _WIN32
    closesocket(server_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}





void p2p_client_1()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsa_data;
    if (WSAStartup(socket_version, &wsa_data) != 0)
    {
        printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_skt == INVALID_SOCKET)
    {
        printf("create socket error.\n");
        return;
    }

    // 用來指定本地端的port
    p2p_bind_client_socket(client_skt);

    // ************************************************ send to server 1 ************************************************
    sockaddr_in remote_addr;
    bzero(&remote_addr, sizeof remote_addr);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(p2p_server_port_1);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr( "36.231.65.243" );
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    socklen_t remote_len = sizeof(remote_addr);

    // send data
    char send_data[30] = "p2p test. send to server 1.";
    ssize_t ret;
    ret = sendto(client_skt, send_data, (int)strlen(send_data), 0, (sockaddr*)&remote_addr, remote_len);
    printf("client send, ret = %ld\n", ret);

    // ************************************************ send to server 2 ************************************************
    bzero(&remote_addr, sizeof remote_addr);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(p2p_server_port_1);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr("122.116.84.59");
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    remote_len = sizeof(remote_addr);

    // send data
    sprintf( send_data, "p2p test. send to server 2." );
    ret = sendto(client_skt, send_data, (int)strlen(send_data), 0, (sockaddr*)&remote_addr, remote_len);
    printf("client send, ret = %ld\n", ret);

#ifdef _WIN32
    closesocket(client_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(client_skt);
#endif
}










void p2p_server_2()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsa_data;
    if (WSAStartup(socket_version, &wsa_data) != 0)
    {
        printf("init error\n");
        return;
    }
#endif

    // bind server socket
    SOCKET server_skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    sockaddr_in local_addr;
    bzero(&local_addr, sizeof local_addr);  // windows沒有bzero
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(p2p_server_port_2);

#ifdef _WIN32
    local_addr.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr("111.248.195.94");
#elif defined(UNIX) || defined(MACOS)
    local_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    int local_len = sizeof(local_addr);

    // https://man7.org/linux/man-pages/man2/bind.2.html   see how to handle error
    if (bind(server_skt, (sockaddr*)&local_addr, local_len) == SOCKET_ERROR)
    {
        printf("bind error !\n");
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }

    printf("bind success. wait for client data...\n");

    // ************************************ recv from client 1 ************************************
    sockaddr_in remote_addr_1;
    bzero(&remote_addr_1, sizeof remote_addr_1);
    socklen_t remote_len_1 = sizeof(remote_addr_1);

    // 
    char recv_data[30] = { 0 };
    ssize_t ret;
    ret = recvfrom(server_skt, recv_data, sizeof recv_data, 0, (sockaddr*)&remote_addr_1, &remote_len_1);
    recv_data[ret] = '\0';

    if (ret > 0)
    {
        printf("recv from ip = %s, port = %d\n", inet_ntoa(remote_addr_1.sin_addr), ntohs(remote_addr_1.sin_port));
        printf("recv data. ret = %ld, recv_data = %s\n", ret, recv_data);
    }
    else
    {
        printf("ret = %ld, error\n", ret);
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }
    printf("\n");

    // ************************************ recv from client 2 ************************************
    sockaddr_in remote_addr_2;
    bzero(&remote_addr_2, sizeof remote_addr_2);
    socklen_t remote_len_2 = sizeof(remote_addr_2);

    // 
    ret = recvfrom(server_skt, recv_data, sizeof recv_data, 0, (sockaddr*)&remote_addr_2, &remote_len_2);
    recv_data[ret] = '\0';

    if (ret > 0)
    {
        printf("recv from ip = %s, port = %d\n", inet_ntoa(remote_addr_2.sin_addr), ntohs(remote_addr_2.sin_port));
        printf("recv data. ret = %ld, recv_data = %s\n", ret, recv_data);
    }
    else
    {
        printf("ret = %ld, error\n", ret);
#ifdef _WIN32
        closesocket(server_skt);
        WSACleanup();
#elif defined(UNIX) || defined(MACOS)
        close(server_skt);
#endif
        return;
    }

    // ************************************ send back ************************************
    char msg1[60] = {0}, msg2[60] = {0};

    sprintf( msg1, "%s %d", inet_ntoa(remote_addr_1.sin_addr), ntohs(remote_addr_1.sin_port) );
    printf( "msg1 = %s\n\n", msg1 );
    sendto( server_skt, msg1, 60, 0, (sockaddr*)&remote_addr_2, remote_len_2 );

    sprintf(msg2, "%s %d", inet_ntoa(remote_addr_2.sin_addr), ntohs(remote_addr_2.sin_port));
    printf("msg2 = %s\n\n", msg2);
    sendto(server_skt, msg2, 60, 0, (sockaddr*)&remote_addr_1, remote_len_1);


#ifdef _WIN32
    closesocket(server_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(server_skt);
#endif
}





void p2p_client_2()
{
#ifdef _WIN32
    // windows need init
    WORD socket_version = MAKEWORD(2, 2);
    WSADATA wsa_data;
    if (WSAStartup(socket_version, &wsa_data) != 0)
    {
        printf("init error\n");
        return;
    }
#endif

    // create socket
    SOCKET client_skt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_skt == INVALID_SOCKET)
    {
        printf("create socket error.\n");
        return;
    }

    // 用來指定本地端的port
    //p2p_bind_client_socket(client_skt);

    // ************************************************ send to server ************************************************
    sockaddr_in remote_addr;
    bzero(&remote_addr, sizeof remote_addr);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(p2p_server_port_2);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr("36.231.65.243");
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    socklen_t remote_len = sizeof(remote_addr);

    // send data
    char send_data[30] = "p2p test. send to server 1.";
    ssize_t ret;
    ret = sendto(client_skt, send_data, (int)strlen(send_data), 0, (sockaddr*)&remote_addr, remote_len);
    printf("client send, ret = %ld\n", ret);

    // ************************************************ recv from server ************************************************
    char recv_data[60] = {0};
    ret = recvfrom( client_skt, recv_data, 60, 0, (sockaddr*)&remote_addr, &remote_len );

    char device_ip[30];
    int device_port = 0;
    printf( "recv msg = %s\n", recv_data );
    sscanf( recv_data, "%s %d", device_ip, &device_port );
    printf( "device ip = %s, device port = %d\n", device_ip, device_port );

    // ************************************************ start p2p ************************************************
    bzero(&remote_addr, sizeof remote_addr);
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(device_port);
#ifdef _WIN32
    remote_addr.sin_addr.S_un.S_addr = inet_addr(device_ip);
#elif defined(UNIX) || defined(MACOS)
    remote_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    //inet_pton( AF_INET, "127.0.0.1", &servaddr.sin_addr );  // 另一個作法
#endif
    remote_len = sizeof(remote_addr);

    char msg1[60];
    char msg2[60];
    int count = 0;
    while(true)
    {
        sprintf( msg1, "p2p. count = %d", count++ );
        ret = sendto( client_skt, msg1, 60, 0, (sockaddr*)&remote_addr, remote_len );

        ret = recvfrom( client_skt, msg2, 60, 0, (sockaddr*)&remote_addr, &remote_len);
        printf( "msg2 = %s, from %s %d\n", msg2, inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port) );

        Sleep(500);
    }


#ifdef _WIN32
    closesocket(client_skt);
    WSACleanup();
#elif defined(UNIX) || defined(MACOS)
    close(client_skt);
#endif
}