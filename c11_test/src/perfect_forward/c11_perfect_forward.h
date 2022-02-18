#ifndef C11_PERFECT_FORWARD_H
#define C11_PERFECT_FORWARD_H


/*
https://codinfox.github.io/dev/2014/06/03/move-semantic-perfect-forward/  移動語意 完美轉發 部分翻譯有錯 

http://thbecker.net/articles/rvalue_references/section_08.html
http://thbecker.net/articles/rvalue_references/section_07.html
http://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/

http://hungmingwu-blog.logdown.com/posts/14290-rvalue-reference

https://tjsw.medium.com/%E6%BD%AE-c-11-perfect-forwarding-%E5%AE%8C%E7%BE%8E%E8%BD%89%E7%99%BC%E4%BD%A0%E7%9A%84%E9%9C%80%E6%B1%82-%E6%B7%B1%E5%BA%A6%E8%A7%A3%E6%9E%90-f991830bcd84
*/



/*
    左值可以用 & 取得記憶體位置, 右值不行

    左值 放在記憶體內
    右值 放在暫存器內 (某個地方看到的記憶方式,但不確定正確性.需要參考組語)

    因為右值參考傳入後,可以用 & 取得記憶體位置, 會變左值
    用std::forward保持左值右值特性, 稱為 perfect forward.
*/


namespace c11 {




void perfect_forward_test_1();
void perfect_forward_test_2();
void perfect_forward_test_3();
void perfect_forward_test_4();
void perfect_forward_test_5();
void perfect_forward_test_6();


void pf_test_funcs();


} // end namespace c11





#endif