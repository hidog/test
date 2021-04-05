#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <string.h>
#include <arpa/inet.h>

#define SERV_PORT 1234

#define MAXNAME 4096

extern int errno;

main()
{
    int socket_fd;   
    int recfd; 
    int length; 
    int nbytes; 
    char buf[BUFSIZ];
    
    struct sockaddr_in myaddr;
    struct sockaddr_in client_addr; 


       if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) <0) {
                perror ("socket failed");
                exit(EXIT_FAILURE);
        }
        /*
         *    Set up our address
         */
        bzero ((char *)&myaddr, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(SERV_PORT);

        /*
         *     Bind to the address to which the service will be offered
         */
        if (bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) <0) {
                perror ("bind failed\n");
                exit(1);
        }

        /*
         * Loop continuously, waiting for datagrams
         * and response a message
         */
        length = sizeof(client_addr);
        printf("Server is ready to receive !!\n");
        printf("Can strike Cntrl-c to stop Server >>\n");
        while (1) {
                if ((nbytes = recvfrom(socket_fd, &buf, MAXNAME, 0, (struct sockaddr*)&client_addr, (socklen_t *)&length)) <0) {
                        perror ("could not read datagram!!");
                        continue;
                }


                printf("Received data form %s : %d\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
                printf("%s\n", buf);

                /* return to client */
                if (sendto(socket_fd, &buf, nbytes, 0, (struct sockaddr*)&client_addr, length) < 0) {
                        perror("Could not send datagram!!\n");
                        continue;
                }
                printf("Can Strike Crtl-c to stop Server >>\n");
        }
}
