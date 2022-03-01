#ifndef CAST_TEST_H
#define CAST_TEST_H



/*

https://b8807053.pixnet.net/blog/post/339782825-%5B轉%5D-%5Bc%2B%2B%5D-標準類型轉換：static_cast%2C-dynamic_cast%2C-
http://windsplife.blogspot.com/2010/09/cstaticcast-dynamiccast-reinterpretcast.html
https://docs.microsoft.com/zh-tw/cpp/cpp/reinterpret-cast-operator?view=msvc-160
 
static_cast   
一般用於基本型態的轉型   int, double, char等
可以用在class的轉型, 兩個沒繼承關係的class不能轉. (至少xcode的行為是如此,其他編譯器不確定)
如果是有繼承關係的class可轉, 但型態不對的時候會造成未知的錯誤

dynamic_cast
一般用在繼承關係的class轉型.
繼承關係錯誤的情況下轉型會得到nullptr. 有基本的檢查型態機制

reinterpret_cast  
可以任意轉型,包含不同型態的指標硬轉
例如用 char 印出 int64_t 的每個 byte 的資料
 
const cast
可以將const傳入參數的const屬性消除
但如果原始變數是const,原始變數還是無法改值
一般用在func傳入const ref但仍須改值時的一個work around.
亦可用在class的const function.

*/


void static_cast_test();
void dynamic_cast_test();
void reinterpret_cast_test();
void const_cast_test();
void const_sub( const int& a );

void cast_test_func();

#endif
