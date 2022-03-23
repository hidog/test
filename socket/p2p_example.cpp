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