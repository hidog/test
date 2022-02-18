#include "thread_test_1.h"
#include "thread.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#if defined(UNIX) || defined(MACOS)
#include <unistd.h>
#endif


#define MAX_THREADS 5


int g_count = 0;
THREAD_LOCK lock;


struct ThreadData
{
    unsigned int thr_id;
    int thr_index;
};




#ifdef WIN32
DWORD WINAPI run_data_count( LPVOID param )
#elif defined(UNIX) || defined(MACOS)
void* run_data_count( void* param )
#else
#error not defined
#endif
{
    printf("start run_data_count\n");

    struct ThreadData *data = (struct ThreadData*)param;
    int     i;

    printf("thread index = %d\n", data->thr_index );
    
    for( i = 0; i < 20; i++ )
    {
        int r = rand() % 5 + 5;
        //printf( "r = %d\n", r );
        
        thread_lock( &lock );


        int tmp = g_count;
        // 為了觸發race condition,刻意讓thread小睡. 不加上sleep不好觸發.
#ifdef WIN32
        Sleep(r);
#else
        usleep( 1000*r );
#endif
        g_count = tmp + 10;


        thread_unlock( &lock );
    }

    thread_exit();
    return 0;
}




int test_thread_1(void)
{
    srand( (unsigned)time(0) );

    THREAD_ID       thread_id[MAX_THREADS];
    THREAD_HANDLE   thread[MAX_THREADS]; 
    struct ThreadData   data[MAX_THREADS];

    int     i;

    thread_global_init();

    thread_init_lock( &lock );

    // 
    for( i = 0; i < MAX_THREADS; i++ )
    {
        data[i].thr_index = i;

        thread[i] = my_thread_create( run_data_count, &data, &thread_id[i] );

#ifdef MACOS
        data[i].thr_id = i;
#else
        data[i].thr_id = thread_id[i];
#endif
        printf("thr id = %d\n", data[i].thr_id );

#ifdef WIN32
        if( thread[i] == NULL ) 
#elif defined(UNIX) || defined(MACOS)
        if( thread[i] == 0 )
#else
#error not defined.
#endif
        {
            printf( "error !!!" );
            return -1;
        }
    }

    //
    thread_join( thread, MAX_THREADS );

    printf( "result cound = %d, expect value is %d\n", g_count, MAX_THREADS*10*20 );  //

    //
    for( i = 0; i < MAX_THREADS; i++ )
        thread_close( thread[i] );
    
    thread_close_lock( &lock );
    thread_global_finish();
    //thread_exit();

    return 0;
}
