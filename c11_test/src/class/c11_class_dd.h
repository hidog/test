#ifndef C11_CLASS_DD_H
#define C11_CLASS_DD_H


// DD = delete default, 用來測試c++11 class 預設函數 = delete 或 default.


/*
https://kheresy.wordpress.com/2014/10/09/c11-default-and-delete/
http://www.cnblogs.com/oomusou/archive/2007/01/17/622428.html
*/




class ClassDelete_1
{
public:
    ClassDelete_1() = default;
    ClassDelete_1( const ClassDelete_1& s ) = delete;
    ClassDelete_1( ClassDelete_1&& s ) = delete;
    ClassDelete_1& operator = ( ClassDelete_1&& s ) = delete;
    ClassDelete_1& operator = ( const ClassDelete_1& s ) = delete;
    ~ClassDelete_1() = default;

    void test();

private:


};



class ClassDelete_2
{
public:
    ClassDelete_2();
    ClassDelete_2( const ClassDelete_2& s );
    ClassDelete_2( ClassDelete_2&& s ) = delete;
    ClassDelete_2& operator = ( ClassDelete_2&& s ) = delete;
    ClassDelete_2& operator = ( const ClassDelete_2& s );
    ~ClassDelete_2() = default;

    void print();

private:
    int data;
};




class ClassDefault
{
public:
    // 書上提到使用default有機會讓編譯器最佳化. (相對於自己撰寫的簡易版本來講)
    ClassDefault() = default;
    ClassDefault( const ClassDefault& ) = default;
    ClassDefault( ClassDefault&& ) = default;
    
    ClassDefault& operator = ( const ClassDefault& ) = default;
    ClassDefault& operator = ( ClassDefault&& ) = default;

    void init();
    void print();

    ~ClassDefault() = default;

    int *data;
};





namespace c11 {


void class_delete_test_1();
void class_delete_test_2();

void class_default_test();


void class_dd_funcs();


} // end namespace c11






#endif