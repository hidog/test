#ifndef THREAD_TEST_H
#define THREAD_TEST_H

#include <string>
#include <memory>


/*
    原本想參考這篇 https://shengyu7697.github.io/std-atomic/ 
    但無奈無法製造出race condition. (推測是執行速度太快)
    最後選擇放棄,僅就理論上討論atomic.

    Semaphore   可以同時多個thread進入區間, 也可以由不同人解鎖
    https://jasonblog.github.io/note/linux_system/mutex_yu_semaphore_zui_da_de_cha_yi_shi.html
    https://medium.com/@chinhung_liu/work-note-semaphore-mutex-spinlock-391bba4c161a

    https://welkinchen.pixnet.net/blog/post/47071066-spinlock-%26-mutex-%26-semaphore-%E7%9A%84%E4%BD%9C%E7%94%A8%E5%92%8C%E5%8D%80%E5%88%A5
    https://blog.hitripod.com/synchronization-mechanism-comparison-spinlock-mutex/

    https://hackmd.io/@sysprog/multicore-locks?type=view
    https://ithelp.ithome.com.tw/articles/10213132

    https://shengyu7697.github.io/std-ref/

    semaphore要等c++20
    spinlock可以參考atomic_flag
*/



void thr_basic_func_1();
void thread_basic_test_1();

void thr_basic_func_2( int index );
void thread_basic_test_2();

void thr_basic_func_3( std::string& str, int index );
void thread_basic_test_3();



void thread_sleep_test();



void thread_c11_test_1();
void thread_c11_test_2();

void thr_c11_test_3( std::shared_ptr<int> p );
void thread_c11_test_3();

void thread_c11_test_4();



void thread_obj_test_1();
void thread_obj_test_2();
void thread_obj_test_3();





void thread_entry_func();




#endif
