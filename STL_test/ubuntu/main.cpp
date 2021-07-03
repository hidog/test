#include <iostream>
#include <cassert>

#include "../thread.h"
#include "../macro.h"


using namespace std;
 
 


// https://en.cppreference.com/w/cpp/preprocessor/replace
// 參考這個網址 紀錄想實驗的程式碼


int main()
{    
    // thread
    c99_thread_test();

    // macro
    x64_x86_macro();
    c11_macro();


        
        
    assert(0);


    return 0;
}
