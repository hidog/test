#include "typeid.h"
#include <iostream>

using namespace std;


class TestClass
{
public:
    /* 
        沒有virtual destructor, mac環境, 
        typeidf執行結果印出的都是基底class.
        加上virtual destructor後, 會因為衍生物間產生不同的執行結果.
     */
    virtual ~TestClass() = default; 
    int data;
};



class TestDerive : public TestClass 
{
public:
    int data2;
};




void test_typeid_1()
{
    string a = typeid(int).name();  
    cout << "typeid(int) = " << a << endl;

    string b = typeid(TestClass).name();
    cout << "typeid(TestClass) = " << b << endl;
}






void test_typeid_2()
{
    TestClass *base = nullptr;
    base = new TestClass;
    cout << typeid(*base).name() << endl;
    delete base;
    
    base = new TestDerive;
    cout << typeid(*base).name() << endl;
    delete base;
}
