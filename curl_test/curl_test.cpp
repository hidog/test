#include <stdio.h>
#include <curl/curl.h>
#include <string>
#include <fstream>
#include "curl_test.h"

#define SKIP_PEER_VERIFICATION
#define SKIP_HOSTNAME_VERIFICATION


int writer_https(char *data, size_t size, size_t nmemb, std::string *writerData)
{
    if (writerData == NULL)
        return 0;

    writerData->append(data, size*nmemb);

    return size * nmemb;
}



void test_curl_https()
{
    CURL *curl;
    CURLcode res;

    std::string buffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if( nullptr != curl ) 
    {
        curl_easy_setopt( curl, CURLOPT_URL, "https://example.com/" );

#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_https);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)        
            fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            printf( "%s", buffer.c_str() );

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}






void test_curl_http_post()
{
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if( curl != nullptr )
    {
        // 這個網址不存在, 有空改寫一個自己用的測試伺服器.
        curl_easy_setopt(curl, CURLOPT_URL, "http://postit.example.com/moo.cgi");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "name=daniel&project=curl");
        res = curl_easy_perform(curl);

        if( res != CURLE_OK )
            fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}







size_t write_data( void *buffer, size_t size, size_t nmemb, void *userp )
{
    HttpData *http_data = (HttpData*)userp;
    http_data->buffer_count += size * nmemb;
    printf( "%d %s\n", http_data->buffer_count, http_data->url );
    fwrite( buffer, size, nmemb, http_data->file );
    return size * nmemb;
}




void test_curl_download_image()
{
    CURL *curl;
    CURLcode res;

    HttpData http_data;
    http_data.buffer_count = 0;
    http_data.file = NULL;
    strcpy( http_data.url, "https://images.pexels.com/photos/325185/pexels-photo-325185.jpeg" );

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if( nullptr != curl ) 
    {
        curl_easy_setopt( curl, CURLOPT_URL, http_data.url );

#if 1
#ifdef SKIP_PEER_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
#endif

        http_data.file = fopen("test.jpg", "wb+");

        res = curl_easy_setopt( curl, CURLOPT_WRITEDATA, &http_data );
        res = curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, write_data );

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)        
            fprintf( stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else        
            printf( "finish !\n" );        

        curl_easy_cleanup(curl);
        fclose(http_data.file);
    }

    curl_global_cleanup();
}
