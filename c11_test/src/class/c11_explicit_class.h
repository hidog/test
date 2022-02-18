#ifndef C11_EXPLICIT_CLASS_H
#define C11_EXPLICIT_CLASS_H



class IntegerTest
{
public:
    //IntegerTest( int a );
    /*
        當 constructor 只有一個參數的時候
        可以使用 IntegerTest it = 3; 這樣的語法
        加上explicit後這個語法會造成編譯錯誤.
        一般建議加上explicit的理由是避免誤用constructor.
    */
    explicit IntegerTest( int a );  

    void print();

private:
    int data;
};



namespace c11 {

void test_explicit_class();

} // end namespace c11


#endif