#ifndef THREAD_ENTRY_H
#define THREAD_ENTRY_H

/*
https://docs.microsoft.com/en-us/windows/win32/procthread/creating-threads
https://docs.microsoft.com/zh-tw/windows/win32/sync/using-critical-section-objects
https://docs.microsoft.com/zh-tw/windows/win32/sync/condition-variables
https://docs.microsoft.com/en-us/windows/win32/sync/using-mutex-objects
https://docs.microsoft.com/en-us/windows/win32/procthread/creating-threads

win api 提供: mutex, critical section兩個方式來做lock


https://www.796t.com/post/ZDFhdzQ=.html
https://hpc-tutorials.llnl.gov/posix/#Abstract


這個是舊的程式碼
僅記錄一些網址,變數名稱,執行上有點問題,有空再研究.



https://medium.com/@racktar7743/c%E8%AA%9E%E8%A8%80-function-pointer%E7%9A%84%E6%87%89%E7%94%A8-%E4%B8%89-%E4%BD%BF%E7%94%A8-typdef-%E4%BE%86%E5%AE%9A%E7%BE%A9%E5%87%BD%E6%95%B8%E6%8C%87%E6%A8%99%E4%BB%A5%E5%A2%9E%E5%8A%A0%E7%A8%8B%E5%BC%8F%E7%9A%84%E5%8F%AF%E8%AE%80%E6%80%A7-7a26857e3e00
function pointer. 

我們如果要宣告一個函數指標，我們會使用下面的方法宣告
int (*calculation)(int, int) = func;
而如果我們要對函數指標使用 typedef 的話，直接對宣告的式子前面加上typedef，這樣子我們就可以使用calculation這個名稱來宣告函數
typedef int (*calculation)(int, int);

*/




#ifdef __cplusplus
extern "C" {
#endif



int c_thread_test(void);





#ifdef __cplusplus
} // end extern "C"
#endif


#endif