#include "smart_point.h"
#include "my_array.h"
#include "sp_class.h"

#include <iostream>

using namespace std;













void test_unique_pt_1()
{
    //
    std::unique_ptr<int> ptr1( new int {1234} ); 
    //std::unique_ptr<int> ptr1 = new int;  // compile fail.
    cout << *ptr1 << endl;
    
    //
    // unique_ptr<double> ptr2{ new double[100] { 0.1, 0.2, 0.3, 0.4 } }; // 會造成無法用 [] 存取
    unique_ptr<double[]> ptr2{ new double[100] { 0.1, 0.2, 0.3, 0.4 } };
    for( int i = 0; i < 10; i++ )
        cout << ptr2[i] << " ";
    cout << endl;
    
    //
    //unique_ptr<short> const ptr3 = unique_ptr<short>( new short {12} );
    //const unique_ptr<short> ptr3 = unique_ptr<short>( new short {12} );
    unique_ptr<const short> ptr3 = unique_ptr<short>( new short {12} ); // 只有這個會造成改值失敗,剩下的都不影響修改值
    //*ptr3 = 99;
    cout << *ptr3 << endl;
    
    //
    auto ptr4 = make_unique<char>('Z');
    char* ptr5 = ptr4.get();
    *ptr5 = 'D';
    cout << *ptr4 << endl; // ptr4的值也會一起被修改
    
    //
    unique_ptr<int> ptr6;
    
    if( ptr4 )
        cout << "ptr4 not nullptr.\n";
    
    if( !ptr6 )
        cout << "ptr6 is nullptr\n";
        
    if( ptr4 != nullptr )
        cout << "ptr4 not nullptr\n";
    
    if( ptr6 == nullptr )
        cout << "ptr6 is nullptr\n";
    
    //if( ptr4 == true ) 編譯錯誤
    if( static_cast<bool>(ptr4) == true )
        cout << "ptr4 not nullptr.\n";
    
    if( static_cast<bool>(ptr6) == false )
        cout << "ptr6 is nullptr\n";
}




void test_unique_pt_2()
{
    std::unique_ptr<int> p1(new int(42));
    std::unique_ptr<int> p2(new int(42));
    std::unique_ptr<int> p3(new int(43));
    
    std::unique_ptr<int>& p4 = p1;
 
    // 這幾個是比對記憶體位置.
    if( p1 == p4 )
        cout << "p1 == p4\n";
    
    if( p1 < p3 )
        cout << "p1 < p3. p1 addr = " << p1.get() << ", p3 addr = " << p3.get() << endl;
    else if( p1 > p3 )
        cout << "p1 > p3 p1 addr = " << p1.get() << ", p3 addr = " << p3.get() << endl;
}




void test_unique_pt_3()
{
    //
    {
        // unique_ptr<SpFoo>   ptr1    =   unique_ptr<SpFoo>{};   // 錯誤語法, 不會進入SpFoo constructor.
        unique_ptr<SpFoo>   ptr1    =   unique_ptr<SpFoo>{ new SpFoo(10,14.23) };
        ptr1->shuffle_array( 100, 200 );
        ptr1->output();
    }
    cout << endl;
    
    //
    auto ptr2   =   make_unique<SpFoo>();
    ptr2->output();
    cout << endl;
}




void test_unique_pt_4()
{
    // 
    unique_ptr<SpFoo> ptr;
    if( ptr == nullptr )
    {
        cout << "ptr3 is null, create new.\n";
        ptr    =     make_unique<SpFoo>( 25, 153.4, 9 );
        ptr->output();
    }
    ptr    =   nullptr; // 會釋放原本的記憶體.
    ptr    =   unique_ptr<SpFoo>{ new SpFoo };
    ptr->shuffle_array( -100, 100 );
    ptr->output();
    cout << endl;
    
    //
    {
        auto ptr2   =   make_unique<SpFoo>( -44123, -0.0002345 );
        SpFoo *point    =   ptr2.release(); 
        cout << "age = " << point->age << ", height = " << *point->height << endl;;
        cout << "unique addr = " << ptr2.get() << endl; // addr is null addr 0x0.
    
        delete point->height; point->height = nullptr;
        delete [] point->array; point->array = nullptr;
        // 不會進 ptr2 的 destructor.
    }
    cout << endl;
    
    //
    unique_ptr<SpFoo> ptr3( new SpFoo( 1, 1.1, 1 ) );
    ptr3->output();
    
    ptr3.reset( new SpFoo( 2, 2.2, 2 ) );
    ptr3->output();
    
    //ptr3.reset(nullptr);
    ptr3.reset();  // 作用同上
}





void test_unique_pt_5()
{
    unique_ptr<SpFoo[]> ptr{ new SpFoo[3]{ {1,1.f,1}, {2,2.f,2}, {3,3.f,3} } };
    ptr[0].output();
    ptr[1].output();
    ptr[2].output();
}




void test_unique_pt_6()
{
    auto ptr1 = make_unique<SpFoo>(1,1,1);
    auto ptr2 = make_unique<SpFoo>(2,2,2);
    
    swap( ptr1, ptr2 );
    ptr1->output();
    ptr2->output();
    
    //
    //ptr1 = ptr2; // compile fail. unique pointer has no copy constructor and assignment.
    ptr2->shuffle_array( 1000, 2000 );
    ptr1 = move(ptr2); // 進去 destructor, 沒進move assignment.
    ptr1->output();
    
    //
    ptr2 = make_unique<SpFoo>(-1,-1,-1);
    //ptr1.reset( ptr2.get() );  // 會造成 destructor 的時候 crash.
    ptr1.reset( ptr2.release() );
    ptr1->output();
}





void test_unique_pt_7()
{
    {
        unique_ptr<SpFoo> ptr1{ new SpBar };
        ptr1->output();
    }
    
    //
    unique_ptr<SpFoo> ptr2 = make_unique<SpBar>( 100, 200, "SuMiNo" );
    ptr2->shuffle_array();
    ptr2->output();
}




void smart_class_basic_test()
{
    SpBar   bar1( 10, 180, "Andy" );
    SpBar   bar2( 20, 280, "Peter" );
    
    bar1.shuffle_array();
    bar2.shuffle_array();
    bar1.output();
    bar2.output();
    
    swap(bar1, bar2);
    bar1.output();
    bar2.output();
    
    bar1 = bar2;
    
    bar1.output();
    bar2.output();
}




void test_unique_pt_8()
{
    const char* name1 = "Andy";
    const char* name2 = "Ilia";
    
    SpBar bar1{ 1, 1.5, name1 }, bar2{ 2, 2.7, name2 };
    SpQux qux{ 2, 2.6, 'X' };
    SpFoo foo{ 100, 100, 100 };
    
    bar2.shuffle_array();
    
    //
    unique_ptr<SpFoo> ptr1 = create_obj<SpFoo>(foo);
    ptr1->output();
    
    unique_ptr<SpFoo> ptr2 = create_obj<SpBar>(bar1);
    ptr1->output();
    
    unique_ptr<SpFoo> ptr3 = create_obj<SpBar>( bar1 + bar2 );
    ptr3->output();

    unique_ptr<SpFoo> ptr4 = create_obj<SpQux>(qux);
    ptr4->output();
}





void test_shared_pt()
{
    shared_ptr<SpFoo> p{ new SpBar };
    p->output();
    
    //
    auto func1 = []() -> shared_ptr<SpFoo> {
        auto ptr = make_shared<SpFoo>( 10, 10, 10 );
        return ptr;
    };
    
    auto func2 = [] ( shared_ptr<SpFoo> p ) {
        cout << "use cound = " << p.use_count() << endl;
        p->output();
    };
    
    {
        auto ptr1 = func1();
        auto ptr2 = ptr1;
        auto ptr3 = ptr2;
        auto ptr4 = ptr1;
        auto ptr5 = ptr1;
    
        ptr4->shuffle_array();  
        ptr3->output();
        
        func2(ptr2);
    
        ptr1 = nullptr; 
        ptr2 = nullptr; 
        ptr3 = nullptr; 
        ptr4 = nullptr; 
        ptr5 = nullptr;  
        // 會進destructor. 如果沒人工assign成nullptr,會在結束scope的時候進destructor.
        cout << "test test_shared_pt\n";
    }
    cout << "finish test_shared_pt\n";
}






void test_weak_pt_1()
{
    weak_ptr<SpFoo> w1;
    
    {
        shared_ptr<SpFoo> tmp( new SpFoo );
        w1 = tmp;
    }
    
    shared_ptr<SpFoo> s1 = w1.lock();
    if( s1 != nullptr )
        s1->output();
    else
        cout << "s1 is nullptr\n"; // 此時已經被釋放了
    
    //w1->output(); // 編譯失敗
}





void test_weak_pt_2()
{
    
    weak_ptr<SpFoo> w1;
    
    {
        static shared_ptr<SpFoo> tmp( new SpFoo );
        w1 = tmp;
    }
    
    cout << (w1.expired() ? "free" : "not free") << endl;
    
    shared_ptr<SpFoo> s1 = w1.lock();    
    cout << "count = " << s1.use_count() << endl;
    if( s1 != nullptr )
        s1->output(); // 尚未釋放,可以印出資料
    else
        cout << "s1 is nullptr\n"; 
    
    //w1->output(); // 編譯失敗
}







void test_smart_point_recursive()
{
    shared_ptr<SpQux> ptr1{ new SpQux{1,1.2,'A'} };
    shared_ptr<SpQux> ptr2( new SpQux{2,2.4,'b'} );
    
    ptr2->shuffle_array();
    
#if 0
    /*
        互相引用, 造成 ptr1 等 ptr2 釋放, 
        ptr2 等 ptr1 釋放
        最後沒有順利釋放掉記憶體,造成memory leak.
     */
    ptr1->next = ptr2;
    ptr2->next = ptr1;
    
    ptr1->output();
    ptr2->next->output();
#else
    // 用weak point的話, 計數不會增加, 會正常釋放SpQux, 不會造成memory leak
    ptr1->w_next = ptr2;
    ptr2->w_next = ptr1;
    
    auto p2 = ptr1->w_next.lock();
    if( p2 != nullptr )
        p2->output();  // 執行 ptr2的output.
    
    if( auto p1 = p2->w_next.lock() ) // 並不是很喜歡這種語法, 測試用而已
        p1->output();
    
#endif
}







void test_smart_point()
{
    smart_class_basic_test();
    cout << endl;
    
    test_unique_pt_1();
    cout << endl;
    
    test_unique_pt_2();
    cout << endl;
    
    test_unique_pt_3();
    cout << endl;
    
    test_unique_pt_4();
    cout << endl;
    
    test_unique_pt_5();
    cout << endl;
    
    test_unique_pt_6();
    cout << endl;
    
    test_unique_pt_7();
    cout << endl;
    
    test_unique_pt_8();
    cout << endl;
    
    test_shared_pt();
    cout << endl;
    
    test_weak_pt_1();
    cout << endl;
    
    test_weak_pt_2();
    cout << endl;
    
    test_smart_point_recursive();
    cout << endl;
}
