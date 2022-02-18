#ifndef CURL_TEST_H
#define CURL_TEST_H

#include <string>

/* 

https://ithelp.ithome.com.tw/articles/10160903

有空參考這個網頁做測試 https://www.eebreakdown.com/2016/10/curl-http.html

有空的時候架http server做測試

*/


struct HttpData
{
    char    url[1000];
    FILE    *file;
    int     buffer_count;
};



int writer_https(char *data, size_t size, size_t nmemb, std::string *writerData);
size_t write_data( void *buffer, size_t size, size_t nmemb, void *userp );

void test_curl_https();
void test_curl_http_post();

void test_curl_download_image();

#endif