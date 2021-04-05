#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") 


int udp_server()
{
	WSADATA wsaData;
    WORD sockVersion = MAKEWORD(2,2);
    if( WSAStartup(sockVersion, &wsaData) != 0 )
    {
		printf("error");
        return 0;
    }

    SOCKET serSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); 
    if(serSocket == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(12345);
    serAddr.sin_addr.S_un.S_addr = INADDR_ANY;
    if( bind(serSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR )
    {
        printf("bind error !");
        closesocket(serSocket);
        return 0;
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

        char * sendData = "send back";
        sendto( serSocket, sendData, strlen(sendData), 0, (sockaddr *)&remoteAddr, nAddrLen );
    }


    closesocket(serSocket); 
    WSACleanup();
    return 0;
}