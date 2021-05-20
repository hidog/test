#ifndef TOOL_H
#define TOOL_H




void transform_test();
void error_handle_test();
void sockopt_test();

/*
    1. get MTU
    2. TCP如何知道一次可以傳多少資料?
    3. TCP send, non-block,是否會block?
    4. get mac address

    http://www.cs.tau.ac.il/~eddiea/samples/Non-Blocking/tcp-nonblocking-client.c.html
    study        struct hostent *he;
    if ((he=gethostbyname(argv[1])) == NULL) {  // get the host info 
herror("gethostbyname");
exit(1);
        
 */


#endif
