#include "pointer.h"
#include <stdio.h>



void test_pointer()
{
    /*
        簡單的記憶方式
        const int* ptr, const離ptr比較遠,代表遠方不能修改   (遠方資料)
        int* const ptr, const離ptr比較近,代表近方不能修改   (近方ptr)
    */
    int aa = 4;
    int bb = 5;
    const int* ptr = &aa;  // ptr指向對象可以修改,但不能透過ptr修改值
    ptr = &bb;  
    //*ptr = 4;            // 會造成編譯錯誤

    int* const ptr2 = &aa; // ptr2指向位址不能修改,但能透過ptr2修改值
    *ptr2 = 44;
    //ptr2 = &aa;          // 編譯錯誤

    printf("ptr = %d, ptr2 = %d\n", *ptr, *ptr2 );
}
