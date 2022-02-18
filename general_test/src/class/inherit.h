#ifndef INHERIT_H
#define INHERIT_H




class Foo
{
public:
    virtual void f1();
    //void f1();
};



class Bar : public Foo
{
public:
    void f2();
};





void test_inherit_1();
void test_inherit_2();  // 這個是測試會crash用的,執行會當機.
void test_inherit_3();




#endif
