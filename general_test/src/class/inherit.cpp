#include "inherit.h"
#include "adv_array.h"
#include "array.h"
#include <vector>
#include <iostream>

using namespace std;


static int run_count = 0;
//#define MAX_VECTOR_SIZE 100  
// 編譯器建議把macro改constexpr.  
// P.S. constexpr是c++11的語法
constexpr int MAX_VECTOR_SIZE = 100;






void Foo::f1()
{
    cout << "Foo f1\n";
}



void Bar::f2()
{
    cout << "Bar f2\n";
}











class Vector : public std::vector<int>
{
public:
    Vector()
    {
        resize(MAX_VECTOR_SIZE);
        for( int i = 0; i < MAX_VECTOR_SIZE; i++ )
            at(i) = i + run_count;
    }

    virtual ~Vector()
    {
        run_count++;
    }

    void output()
    {
        for( int i = 0; i < MAX_VECTOR_SIZE; i++ )
            std::cout << at(i) << " ";
        std::cout << "\n";
    }
};








void test_inherit_1()
{
    /*
    不加virtual,這邊會沒執行到Array的destructor.
    */
    Array *ptr = new AdvArray;
    ptr->output();    // output 沒宣告成 virtual, 會執行到Array::output. 
    ptr->output2();   // output2 宣告成virtual, 執行衍生物件的output2
    delete ptr;
    ptr = nullptr;

    /*
    這個會執行到Array的destructor.
    */
    AdvArray *ptr2 = new AdvArray;
    ptr2->output();
    ptr2->output2();
    delete ptr2;
    ptr2 = nullptr;

    /*
    這個會執行到Array的destructor.
    */
    AdvArray adv_array;
    adv_array.output();
    adv_array.output2();

    std::vector<int> *a;
    a = new std::vector<int>;
    delete a;
}



void test_inherit_2()
{
#if 0
    while(true)
    {
        /*
            這段code會當掉
            一般來講,避免繼承STL容器.
        */
        std::vector<int> *ptr = new Vector;
        ((Vector*)ptr)->output();  
        delete ptr;
    }
#endif
}





void test_inherit_3()
{
    Foo* ptr = new Bar;
    Bar* ptr2 = dynamic_cast<Bar*>(ptr);   // Foo::f1沒有virtual, 會造成編譯錯誤.
    ptr2->f2();
}



