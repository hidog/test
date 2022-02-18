#include "thread.h"
#include <stdio.h>


#if defined(UNIX) || defined(MACOS)
pthread_attr_t g_attr;
BOOL g_attr_init = FALSE;
#endif



void thread_global_init( void )
{
#if defined(UNIX) || defined(MACOS)
    if( g_attr_init == FALSE )
    {
        pthread_attr_init(&g_attr);
        pthread_attr_setdetachstate( &g_attr, PTHREAD_CREATE_JOINABLE );
        g_attr_init = TRUE;
    }
#endif
}




THREAD_HANDLE my_thread_create( THREAD_FUNC func, THREAD_FUNC_PARAM param, THREAD_ID* thr_id )
{
#ifdef WIN32
    THREAD_HANDLE thr_handle = NULL;

    // Create the thread to begin execution on its own.
    thr_handle = CreateThread( NULL, 0, func, param, 0, thr_id );  
    if( thr_handle == NULL )
        printf("error !! create thread fail !!\n");

    return thr_handle;
#elif defined(UNIX) || defined(MACOS)

    pthread_t thread = 0;
    pthread_create( &thread, &g_attr, func, param );

    *thr_id = thread; // ubuntu環境下,沒有額外定義thread id.

    return thread;

#else
#error not defined
#endif
}




void thread_lock( THREAD_LOCK* lock )
{
#ifdef WIN32
    EnterCriticalSection(lock); 
#elif defined(UNIX) || defined(MACOS)
    pthread_mutex_lock(lock);
#else
#error not defined
#endif
}



void thread_unlock( THREAD_LOCK* lock )
{
#ifdef WIN32
    LeaveCriticalSection(lock);
#elif defined(UNIX) || defined(MACOS)
    pthread_mutex_unlock(lock);
#else
#error not defined
#endif
}



int thread_init_lock( THREAD_LOCK* lock )
{
#ifdef WIN32
    BOOL res = InitializeCriticalSectionAndSpinCount( lock, 0x00000400 );
    return res;
#elif defined(UNIX) || defined(MACOS)
    int res = pthread_mutex_init( lock, NULL );
    return res;
#else
#error not defined.
#endif
}




int thread_close_lock( THREAD_LOCK* lock )
{
#ifdef WIN32
    DeleteCriticalSection(lock);
    return 0;
#elif defined(UNIX) || defined(MACOS)
    int res = pthread_mutex_destroy( lock );
    return res;
#else
#error not defined
#endif
}



int thread_join( THREAD_HANDLE *thread, int thread_count )
{
#ifdef WIN32
    WaitForMultipleObjects( thread_count, thread, TRUE, INFINITE);
    return 0;
#elif defined(UNIX) || defined(MACOS)
    int i;
    for( i = 0; i < thread_count; i++ )
        pthread_join( thread[i], NULL );
    return 0;
#else
#error need defined.
#endif
}




void thread_exit( void )
{
#if defined(UNIX) || defined(MACOS)
    pthread_exit(NULL);
#endif
}



void thread_global_finish( void )
{
#if defined(UNIX) || defined(MACOS)
    pthread_attr_destroy(&g_attr);
#endif
}




int thread_close( THREAD_HANDLE thread )
{
#ifdef WIN32
    CloseHandle(thread);
    return 0;
#else
    return 0;
#endif
}
