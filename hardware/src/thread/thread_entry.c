#include "thread.h"

#include "thread_test_1.h"

#ifdef WIN32
#include "win_thread.h"
#elif defined(UNIX) || defined(MACOS)
#include "pthread_test.h"
#else
#error not defined
#endif

#include <stdio.h>






void c_thread_test(void)
{
#ifdef WIN32
    win_thread_test();
    win_mutex_test();
#elif defined(UNIX) || defined(MACOS)
    pthread_test();
#endif
    printf("\n");

    test_thread_1();
    printf("\n");
}
