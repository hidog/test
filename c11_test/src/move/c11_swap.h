#ifndef C11_SWAP_H
#define C11_SWAP_H


/*
看討論, std::swap原本是交換
可以用在move操作上.
*/

/*
通常operator = 會有 *this != rhs 的問題. (沒處理好會造成crash)
可以用std::swap

copy and swap idiom
want speed? pass by value


class MyClass {
public:
    MyClass(MyString str) :
        str(std::move(str)) { // copy-and-swap/move here
    }
private:
    MyString const str; // Still const!
};
*/


namespace c11 {


void swap_test();


} // end namespace c11



#endif