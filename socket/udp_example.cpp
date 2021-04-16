#include "udp_example.h"
#include <WinSock2.h>
#include <stdio.h>





#if 0
void udp_send()
{
    WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2,2);
    if( WSAStartup(sockVersion, &wsaData) != 0 )
    {
		printf("error");
        return;
    }

    SOCKET serSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); 
    if(serSocket == INVALID_SOCKET)
    {
        printf("socket error !");
        return;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(12345);
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if( bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR )
    {
        printf("bind error !");
        closesocket(serSocket);
        return;
    }
    
    sockaddr_in remoteAddr;
    int nAddrLen = sizeof(remoteAddr); 
    while (true)
    {
        char recvData[255];  
        int ret = recvfrom(serSocket, recvData, 255, 0, (sockaddr *)&remoteAddr, &nAddrLen);
        if (ret > 0)
        {
            recvData[ret] = 0x00;
            printf("recv from : %s \r\n", inet_ntoa(remoteAddr.sin_addr) );
            printf(recvData);            
        }

        char sendData[30] = "send back";
        sendto( serSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen );
    }


    closesocket(serSocket); 
    WSACleanup();
    return;
}
#endif



void udp_send()
{
    WORD socketVersion = MAKEWORD(2,2);
    WSADATA wsaData; 
    if(WSAStartup(socketVersion, &wsaData) != 0)
    {
		printf("error");
        return;
    }
    SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345);
    sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int len = sizeof(sin);
    
    char sendData[30] = "hello, server.";
    sendto(sclient, sendData, strlen(sendData), 0, (sockaddr *)&sin, len);

    /*char recvData[255];     
    int ret = recvfrom(sclient, recvData, 255, 0, (sockaddr *)&sin, &len);
    if(ret > 0)
    {
        recvData[ret] = 0x00;
        printf(recvData);
    }*/

    closesocket(sclient);
    WSACleanup();
}


void udp_recv()
{
	WORD socketVersion = MAKEWORD(2,2);
    WSADATA wsaData; 
    if(WSAStartup(socketVersion, &wsaData) != 0)
    {
		printf("error");
        return;
    }
    SOCKET sclient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345);
    //sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int len = sizeof(sin);
    
    //char * sendData = "hello, server.";
    //sendto(sclient, sendData, strlen(sendData), 0, (sockaddr *)&sin, len);

    char recvData[255];     
    int ret = recvfrom(sclient, recvData, 255, 0, (sockaddr *)&sin, &len);
    if(ret > 0)
    {
        recvData[ret] = 0x00;
        printf(recvData);
    }

    closesocket(sclient);
    WSACleanup();
}