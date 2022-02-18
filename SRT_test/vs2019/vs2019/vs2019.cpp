#include <iostream>

#include "srt_client.h"
#include "srt_server.h"

#include "ffmpeg_server.h"
#include "ffmpeg_client.h"


/*
    _setmode( _fileno(stdin), O_BINARY );
    _setmode( _fileno(stdout), O_BINARY );
    ��stdout, stdin�i�H��binary���覡�ާ@
    �i�H ffmpeg ... | srt_test  �걵���P���{��
*/


#if defined(SERVER_SEND) & defined(CLIENT_SEND)
#error defined error.
#elif defined(SERVER_RECV) & defined(CLIENT_RECV)
#error defined error.
#endif


int main( int argc, char* argv[] )
{
    /*
        ���թ��𵲪G
        * ��}�l���ɭԷ|����, �����������L�hí�w�D�`�h, �[�[�~�|�J��@���j����, 
          �åB�j���𪺼ƾڤ]��L�h�C. (�L�h���𰪫ܦh)
        * �[���쪺�����250ms  (delay = 120ms)
        * �z�פW���ӭn�h�}�@��socket�t�d�^��,���ت��u�O²�����,�N��F
        * �ݭn 20��round sleep�@��, �W�v�U���|�������� 
        * �ݰ_�ө����٬O�L�k�C��10frame. 
        * ����@�q�ɶ���(���@�p�ɧa),�{�Ǧ���,�����D�O�_��sleep�W�v�L�C�y��. ��sleep�L���|�y���y�q����

        * ��ffmpeg, 40M�A������, �T�{�@�U��Ω�
    */
    //srt_client_test();
    //srt_server_test();


    /*
        ����40M�v���@�q�ɶ���,���ݨ���~�T��. (�v�����򼽩�)
        �PıSRT�o���睊���M���@�ǰ��D....
        ������V���ɭԥt�~�@��|��broken pipe line���~,���ޥL
        (�ѨM�覡�O,�_�u���ɭԶ��O�����s���A���n���_�s�u)
        �o�ӬO���եε{��,�@�ǲӸ`�S���B�z�����Y��,�����|�b�קa

        �ϥΤ覡�аѦ�
        
        vs2019.exe 122.116.84.59 1234 | ffplay -                                                      (client recv)
        ffmpeg -stream_loop -1 -re -i test.mp4 -bf 0 -b:v 40M -f mpegts - | vs2019.exe 1234           (server send)

        vs2019.exe 1234 | ffplay -                                                                    (server recv)
        ffmpeg -stream_loop -1 -re -i test.mp4 -f mpegts - | vs2019.exe 122.116.84.59 1234            (client send)

                                        ffplay - -fflags nobuffer  �[�W�o�ӥi�H����buffer,������~��|��í
    */
    std::string ip, port;

    if( argc == 2 )
    {
        port = std::string(argv[1]);
        ffmpeg_server(port);
    }
    else if( argc == 3 )
    {
        ip = std::string(argv[1]);
        port = std::string(argv[2]);
        ffmpeg_client(ip,port);
    }

    return 0;
}




/*
����study�U��lock���Ϊk

void output_to_stdout()
{
    while(true)
    {
        if( g_list.empty() == true )
            Sleep(1);
        else
        {
            std::unique_lock<std::mutex> lck(g_mtx,std::defer_lock);
            lck.lock();
            char *ptr = g_list.front();
            g_list.pop_front();
            lck.unlock();

            fwrite( ptr, 1316, 1, stdout );
            delete [] ptr;
        }
    }
}
*/

