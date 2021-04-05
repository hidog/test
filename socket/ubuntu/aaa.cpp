#include <stdio.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
  


typedef int DWORD;
typedef unsigned long long UINT64;



/*
缺另一端的code
*/




  
int main()  
{  
    struct sockaddr_in server;  
    int sock;  
    int n;  
  
    /* 製作 socket */  
    sock =socket( AF_INET, SOCK_STREAM, 0 );  
  
    /* 準備連線端指定用的 struct 資料 */  
    server.sin_family = AF_INET;  
    server.sin_port = htons(22220);  
  
    /* 127.0.0.1 是 localhost 本機位址 */  
    inet_pton( AF_INET, "192.168.1.1", &server.sin_addr.s_addr );  
  
    /* 與 server 端連線 */  
    int res = connect( sock, (struct sockaddr *)&server, sizeof(server) );  
    printf( "[Info] conncet result = %d\n", res );

    
    n = write( sock, &gt, sizeof(gt) );
    printf( "[Info] send %d bytes.\n", n );
    
  
    /* 從伺服器接受資料 */ 
    


        
            n = read( sock, buf + shift, sizeof(buf) - shift );  

        
  
    //printf( "[Info] Receive %d bytes: %s\n", n, buf );  
  
    /* 結束 socket */  
    close(sock);  
    return 0;  
}
