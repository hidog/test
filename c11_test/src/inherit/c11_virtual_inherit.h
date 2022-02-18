#ifndef C11_VIRTUAL_INHERIT_H
#define C11_VIRTUAL_INHERIT_H



/*
https://openhome.cc/Gossip/CppGossip/virtualInheritance.html

計算大小參考
https://www.itread01.com/content/1546107316.html
https://blog.csdn.net/u014214389/article/details/79760478
https://www.796t.com/post/NHR3dTA=.html

比較完整的計算大小說明
https://developer.aliyun.com/article/465515

*/


#define VI_FUNC



class VI_Base
{
public:
    VI_Base();

#ifdef VI_FUNC
    virtual void exec();
#endif

    int data;
};



class VI_Foo_1 : public virtual VI_Base
//class VI_Foo_1 : public VI_Base
{
public:
    VI_Foo_1();

#ifdef VI_FUNC
    virtual void exec_1();
#endif

    int data_1;
};



class VI_Foo_2 : public virtual VI_Base
//class VI_Foo_2 : public VI_Base
{
public:
    VI_Foo_2();

#ifdef VI_FUNC
    virtual void exec_2();
#endif

    int data_2;
};


#if 1
class VI_Obj : public VI_Foo_1, public VI_Foo_2 
{
public:
    /* 
        不使用 virual inherit 的情況下
        菱形繼承造成 VI_Base::data ambiguous
    */
    VI_Obj();

#ifdef VI_FUNC
    void exec() override;
    void exec_1() override;
    void exec_2() override;
#endif

    int data_3;
};
#endif


namespace c11 {



void test_virtual_inherit();


};




#endif