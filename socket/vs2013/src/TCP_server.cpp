#include <stdio.h>
#include <winsock2.h>
#include <thread>

#pragma comment(lib,"ws2_32.lib")

int tcp_server()
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
		printf("error");
        return 0;
    }

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY; 
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)    
        printf("bind error !");
    
    if(listen(slisten, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    SOCKET sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
    char revData[255]; 
    while (true)
    {
        printf("wait...\n");
        sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("accept from : %s", inet_ntoa(remoteAddr.sin_addr));
        
        // recv
        int ret = recv(sClient, revData, 255, 0);        
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf(revData);
        }

        // send
        char * sendData = "send back";
        send(sClient, sendData, strlen(sendData), 0);
        closesocket(sClient);
    }
    
    closesocket(slisten);
    WSACleanup();
    return 0;
}


int sub_func( SOCKET skt, int index )
{
    char revData[255]; 

	for( int i = 0; i < 100; i++ )
	{
		// recv
        int ret = recv( skt, revData, 255, 0);        
        if(ret > 0)
        {
            revData[ret] = 0x00;
            printf( "index = %d, recv : %s\n", index, revData );
        }

        // send
        char *sendData = "send back";
        send( skt, sendData, strlen(sendData), 0 );
	}

    closesocket(skt);
	return 0;
}



int tcp_server_multi_thread()
{
	WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
		printf("error");
        return 0;
    }

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY; 
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)    
        printf("bind error !");
    
    if(listen(slisten, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

    SOCKET sClient;
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);
	int client_count = 0;

	std::thread *thr[8];

    while (true)
    {
        printf("wait...\n");
        sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
        if(sClient == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("accept from : %s\n", inet_ntoa(remoteAddr.sin_addr));      

		thr[client_count] = new std::thread( sub_func, sClient, client_count );
		client_count++;
    }
    
    closesocket(slisten);
    WSACleanup();
    return 0;
}




int tcp_server_non_blocking()
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA wsaData;
    if(WSAStartup(sockVersion, &wsaData)!=0)
    {
		printf("error");
        return 0;
    }

    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY; 
    if(bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)    
        printf("bind error !");
    
    if(listen(slisten, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("listen error !");
        return 0;
    }

	fd_set fdSocket;
	FD_ZERO( &fdSocket );
	FD_SET( slisten, &fdSocket );

	fd_set fdRead, fdWrite;


	while (true)
	{
		FD_ZERO( &fdRead );
		fdRead = fdSocket;

		FD_ZERO( &fdWrite );
		fdWrite = fdSocket;

		int nRet = select( 0, &fdRead, &fdWrite, nullptr, nullptr ); // can set timeout by parameter 5
 
		if (nRet <= 0)
			break;

		for( int i = 0; i < (int)fdSocket.fd_count; ++i )
		{
			if ( FD_ISSET(fdSocket.fd_array[i], &fdRead) )
			{
				if ( fdSocket.fd_array[i] == slisten )
				{
					sockaddr_in addrRemote;
					int nAddrLen = sizeof(addrRemote);
					SOCKET sNew = accept( slisten, (sockaddr*)&addrRemote, &nAddrLen);
					FD_SET( sNew, &fdSocket );
					printf("Clietn %s connected\n", inet_ntoa(addrRemote.sin_addr));
				}
				else
				{
					char buffer[1024];
					memset(buffer, 0, 1024);
					int nRecev = recv(fdSocket.fd_array[i], buffer, 1024, 0);
 
					if (nRecev > 0)
					{
						printf("Received Client Msg:%s\n", buffer);
						send(fdSocket.fd_array[i], buffer , strlen(buffer), 0);
					}
					else
					{
						closesocket( fdSocket.fd_array[i] );
						FD_CLR(fdSocket.fd_array[i], &fdSocket);
					}
				}
			}
		}
	}
    
    closesocket(slisten);
    WSACleanup();
    return 0;
}
