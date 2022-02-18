#include "c11_other_test.h"

#include <iostream>
#include <string>
#include <iterator>
#include "class/rule_five_class.h"

using namespace std;










class TestNew
{
public:
    TestNew() = default;
    TestNew( const TestNew& t ) = default;
    TestNew& operator = ( const TestNew& t ) = default;
    
    ~TestNew() 
    {
        printf("test delete\n");
    }
    
    void init()
    {
        for( int i = 0; i < 100; i++ )
            a[i] = i;
    }
    
    void print()
    {
        for( int i = 0; i < 100; i++ )
            printf("%d ", a[i] );
    }
    
private:
    int a[100];
};




namespace c11 {

/*
效果類似 cout 輸出字串
*/
void other_test_1()
{
    ostream_iterator<string> oo{cout};
    *oo = "test 1\n";
    ++oo;
    *oo = "test 2\n";

    // this cause compile error
    //++oo;
    //*oo = 123;
}




void test_new()
{
    int *ptr = new int [300];
    
    new(ptr) TestNew(); // 利用 ptr 的記憶體資料創一個TestNew物件出來
    
    TestNew* ptr2 = (TestNew*)ptr;
    
    ptr2->init();
    ptr2->print();
    
    ptr2->~TestNew();

    delete [] ptr;
}



void error_handle_test()
{
    try 
    {
        Array a(5);
        cout << a[10];
    } 
    catch( std::out_of_range &err )
    {
        cout << "error type = " << typeid(err).name() << endl 
             << "error message = " << err.what() << endl;  
    }
    catch( std::exception &err ) 
    {
        cout << "error type = " << typeid(err).name() << endl 
             << "error message = " << err.what() << endl;
    }
    
}





} // end namespace c11 
