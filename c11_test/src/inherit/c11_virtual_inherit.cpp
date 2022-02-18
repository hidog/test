#include "c11_virtual_inherit.h"
#include <iostream>
using namespace std;


VI_Base::VI_Base()
{
    data = 1;
}



#ifdef VI_FUNC
void VI_Base::exec()
{
    cout << "data = " << data << endl;
    cout << __FUNCTION__ << endl;
}
#endif






VI_Foo_1::VI_Foo_1()
{
    data = 20;
    data_1 = 25;
}


#ifdef VI_FUNC
void VI_Foo_1::exec_1()
{
    cout << "data = " << data << endl;
    cout << "data_1 = " << data_1 << endl;
    cout << __FUNCTION__ << endl;
}
#endif




VI_Foo_2::VI_Foo_2()
{
    data = 50;
    data_2 = 55;
}


#ifdef VI_FUNC
void VI_Foo_2::exec_2()
{
    cout << "data = " << data << endl;
    cout << "data_2 = " << data_2 << endl;
    cout << __FUNCTION__ << endl;
}
#endif



#if 1
VI_Obj::VI_Obj()
{
    data = 300;
    data_1 = 410;
    data_2 = 430;
    data_3 = 550;
}
#endif



#ifdef VI_FUNC
void VI_Obj::exec()  
{
    cout << "data = " << data << endl;
    cout << "data_3 = " << data_3 << endl;
    cout << __FUNCTION__ << endl;
}



void VI_Obj::exec_1()  
{
    cout << "data = " << data << endl;
    cout << "data_1 = " << data_1 << endl;
    cout << __FUNCTION__ << endl;
}



void VI_Obj::exec_2()  
{
    cout << "data = " << data << endl;
    cout << "data_2 = " << data_2 << endl;
    cout << __FUNCTION__ << endl;
}
#endif




namespace c11 {

void test_virtual_inherit()
{

#ifdef VI_FUNC
    VI_Base *ptr = new VI_Obj;
    ptr->exec();

    VI_Obj obj;
    obj.exec_1();
    obj.exec_2();
#endif

    /*
        virtual inherit 會讓物件大小變大
        virtual func會增加function pointer. (物件大小因此變大)
        尚未找到如何計算物件大小的參考資料. 因為少用所以先跳過.
        x64,x86
        win,linux,執行結果不同
    */
    cout << "size of VI_Base = " << sizeof(VI_Base) << endl;
    cout << "size of VI_Foo_1 = " << sizeof(VI_Foo_1) << endl;
    cout << "size of VI_Foo_2 = " << sizeof(VI_Foo_2) << endl;
    cout << "size of VI_Obj = " << sizeof(VI_Obj) << endl;
}




} // end namespace c11 
