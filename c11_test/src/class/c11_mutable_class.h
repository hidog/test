#ifndef C11_MUTABLE_CLASS_H
#define C11_MUTABLE_CLASS_H



class DoubleTest
{
public:
    DoubleTest();
    void add_one() const;
    void print();

private:
    /*
        mutable可以讓他在 add_one 內被修改值
        add_one是function const,一般來講禁止改變成員變數.
    */
    mutable double data = 4.22422;  
    double data2 = 12345;
};




namespace c11 {


void test_mutable_class();



} // end namespace c11


#endif