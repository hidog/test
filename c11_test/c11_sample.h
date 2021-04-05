#ifndef C11_SAMPLE_H
#define C11_SAMPLE_H

/*
    http://www.ahlinux.com/c/23357.html   C++11 常用语法
    http://en.cppreference.com/w/cpp/language/rule_of_three    the rule of three/five/zero
    http://en.cppreference.com/w/cpp/language/move_assignment   move assignment
    http://thbecker.net/articles/rvalue_references/section_01.html   C++ Rvalue References Explained
    http://shininglionking.blogspot.tw/2013/06/c-rvalue-reference.html   a+b+c+d+e
    http://jxq.me/2012/06/06/译详解c右值引用/   中譯
    https://codinfox.github.io/dev/2014/06/03/move-semantic-perfect-forward/   移动语义（move semantic）和完美转发（perfect forward）   部分翻譯有錯
 
    perfect forward
    http://thbecker.net/articles/rvalue_references/section_08.html
 http://thbecker.net/articles/rvalue_references/section_07.html
 http://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
 
 
 http://harttle.com/2015/10/11/cpp11-rvalue.html
 
 http://en.cppreference.com/w/cpp/utility/initializer_list
 https://mbevin.wordpress.com/2012/11/16/uniform-initialization/
 http://my.oschina.net/wangxuanyihaha/blog/183151
 https://kheresy.wordpress.com/2014/10/09/c11-default-and-delete/
 http://blog.csdn.net/gzlaiyonghao/article/details/6564162
 http://www.cnblogs.com/oomusou/archive/2007/01/17/622428.html
 https://en.wikipedia.org/wiki/C%2B%2B11#Uniform_initialization
 
 http://thbecker.net/articles/rvalue_references/section_01.html
 http://eli.thegreenplace.net/2011/12/15/understanding-lvalues-and-rvalues-in-c-and-c
 http://en.cppreference.com/w/cpp/language/reference
 
 
 http://en.cppreference.com/w/cpp/language/move_constructor
 http://en.cppreference.com/w/cpp/language/operators
 http://thbecker.net/articles/rvalue_references/section_01.html
 
 
 http://www.cprogramming.com/c++11/rvalue-references-and-move-semantics-in-c++11.html
 http://www.dutor.net/index.php/2013/11/rvalue-reference-move-semantics-and-perfect-forwarding/
    
 
 http://www.ibm.com/developerworks/cn/aix/library/1307_lisl_c11/
 https://www.zhihu.com/question/22111546
 
 http://hungmingwu-blog.logdown.com/posts/14290-rvalue-reference
 
 追加 swap
 通常operator = 會有 *this != rhs 的問題
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


void c11_sample_1();
void c11_sample_2();
void c11_sample_3();
void c11_sample_4();
void c11_sample_5();
void c11_sample_6();
void c11_sample_7();
void c11_sample_8();
void c11_sample_9();
void c11_sample_10();
void c11_sample_11();
void c11_sample_12();
void c11_sample_13();
void c11_swap();


#endif