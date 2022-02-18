#ifndef SMART_POINT_H
#define SMART_POINT_H


/*
 https://www.796t.com/post/NHRqdWk=.html
 enable_if的一個用法
 
 https://www.itread01.com/content/1543822143.html
 
 https://openhome.cc/Gossip/CppGossip/sharedPtr.html
 嘗試自己實現一個shared ptr
 
 https://kheresy.wordpress.com/2012/03/03/c11_smartpointer_p1/
 https://kheresy.wordpress.com/2012/03/05/c11_smartpointer_p2/
 
 */


// auto_ptr, 建議少用. 在c++新的標準已被棄用


/* 
 看起來在 multi-thread 環境可用 weak pointer,
 避免 shared point 的 count 產生race condition.
 之後看 thread 的時候試著組合 shared ptr, weak ptr, thread來研究.
 */

void smart_class_basic_test();

void test_unique_pt_1();
void test_unique_pt_2();
void test_unique_pt_3();
void test_unique_pt_4();
void test_unique_pt_5();
void test_unique_pt_6();
void test_unique_pt_7();
void test_unique_pt_8();

void test_shared_pt();  // shared point效率較差,使用上要留意

void test_weak_pt_1();    // 配合shared point使用, 不會增加ref count,  有機會指向的物件已經釋放
void test_weak_pt_2();

// 這個會測試循環計數, 驗證真的會因此出錯. 這個例子來講不會釋放記憶體,造成memory leak
// 用weak point可以避免這個問題
void test_smart_point_recursive();



void test_smart_point();


#endif // SMART_POINT_H
