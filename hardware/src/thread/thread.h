#ifndef THREAD_H
#define THREAD_H


#ifdef WIN32
#include <windows.h>
#elif defined(UNIX) || defined(MACOS)
#include <pthread.h>
#else
#error need maintain.
#endif


#if defined(UNIX) || defined(MACOS)
#define BOOL    int
#define TRUE    1
#define FALSE   0
#endif



#ifdef WIN32
typedef HANDLE              THREAD_HANDLE;
typedef LPVOID              THREAD_FUNC_PARAM;
typedef DWORD               THREAD_ID;
typedef CRITICAL_SECTION    THREAD_LOCK;
typedef DWORD(WINAPI *THREAD_FUNC)(LPVOID);
#elif defined(UNIX) || defined(MACOS)
typedef pthread_t           THREAD_HANDLE;
typedef void*               THREAD_FUNC_PARAM;
typedef pthread_t           THREAD_ID;
typedef pthread_mutex_t     THREAD_LOCK;
typedef void* (*THREAD_FUNC)(void*);
#else
#error need maintain.
#endif


void            thread_global_init( void );
void            thread_global_finish( void );

// mac環境下, thread_create撞名, 改成 my_thread_create
THREAD_HANDLE   my_thread_create( THREAD_FUNC fun, THREAD_FUNC_PARAM param, THREAD_ID* thr_id );
int             thread_join( THREAD_HANDLE *thread, int thread_count );
int             thread_close( THREAD_HANDLE thread );
void            thread_exit( void );

int             thread_init_lock( THREAD_LOCK* lock ); 
int             thread_close_lock( THREAD_LOCK* lock );
void            thread_lock( THREAD_LOCK* lock );
void            thread_unlock( THREAD_LOCK* lock );


#endif
