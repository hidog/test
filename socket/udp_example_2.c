#include "udp_example_2.h"

#include <netdb.h>
#include <string.h>
#include <stdio.h>

// 使用qtcreator的時候報錯, flag設c11雖然能解決,卻遇到其他錯誤,所以最後直接定義
#ifndef NULL
#define NULL 0
#endif



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}





void udp_hello_server_2( const char* port )
{
    int ret;

    struct addrinfo hints, *servinfo, *ptr;

    int skt;
    /*int numbytes;
    struct sockaddr_storage their_addr;
    char buf[200];
    socklen_t addr_len;
    char str[INET6_ADDRSTRLEN];*/

    memset(&hints, 0, sizeof hints);

    hints.ai_family     =   AF_UNSPEC; // 設定 AF_INET 以強制使用 IPv4
    hints.ai_socktype   =   SOCK_DGRAM;
    hints.ai_flags      =   AI_PASSIVE; // 使用我的 IP


    ret = getaddrinfo( NULL, port, &hints, &servinfo );
    if( ret != 0)
    {
        printf(  "getaddrinfo: %s\n", gai_strerror(ret) );
        return ;
    }
#if 0

    // 用迴圈來找出全部的結果，並 bind 到首先找到能 bind 的
    for( ptr = servinfo; ptr != NULL; ptr = ptr->ai_next)
    {
        skt = socket( ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if ( skt == -1) {
            perror("listener: socket");
            continue;
          }

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("listener: bind");
        continue;
      }

      break;
    }

    if (p == NULL) {
      fprintf(stderr, "listener: failed to bind socket\n");
      return ;
    }


    freeaddrinfo(servinfo);
    printf("listener: waiting to recvfrom...\n");
    addr_len = sizeof their_addr;

    if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
          (struct sockaddr *)&their_addr, &addr_len)) == -1) {

      perror("recvfrom");
      exit(1);
    }

    printf("listener: got packet from %s\n",

    inet_ntop(their_addr.ss_family,

    get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));

    printf("listener: packet is %d bytes long\n", numbytes);

    buf[numbytes] = '\0';

    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);
#endif


}
