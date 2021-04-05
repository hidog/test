#include <winsock2.h>
#include <stdio.h>

#pragma  comment(lib,"ws2_32.lib")


int tcp_client()
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data; 
    if(WSAStartup(sockVersion, &data) != 0)
    {
		printf("error");
        return 0;
    }

    SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
    
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(sclient);
        return 0;
    }


    char * sendData = "hello, server. i am client.";
    send(sclient, sendData, strlen(sendData), 0);

    char recData[255];
    int ret = recv(sclient, recData, 255, 0);
    if(ret > 0)
    {
        recData[ret] = 0x00;
        printf(recData);
    }
    closesocket(sclient);
    WSACleanup();
    return 0;
}



int tcp_client_multi_thread()
{
    WORD sockVersion = MAKEWORD(2,2);
    WSADATA data; 
    if(WSAStartup(sockVersion, &data) != 0)
    {
		printf("error");
        return 0;
    }

    SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sclient == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); 
    
	if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(sclient);
        return 0;
    }

	for( int i = 0; i < 100; i++ )
	{
		char * sendData = "hello, server. i am client.";
		send(sclient, sendData, strlen(sendData), 0);

		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if(ret > 0)
		{
			recData[ret] = 0x00;
			printf(recData);
		}
	}

    closesocket(sclient);
    WSACleanup();
    return 0;
}