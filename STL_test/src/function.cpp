#include "function.h"

#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;




int target_func( int a, char b, std::string c, double d )
{
    cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << endl;
    cout << "a = " << a << ", b = " << b << ", c = " << c << ", d = " << d << endl;
    
    int sum = 0;
    for(auto ch : c )
        sum += ch;
    
    return sum;
}




void test_less_func()
{
    std::less<int> less_int;
    if( less_int(10,100) == true ) 
        std::cout << "true" << std::endl;
}



std::string TargetObj::obj_func( int x, int y )
{
    std::string res;
    res = std::string("The function result = (") + std::to_string(x + a) + "," + std::to_string(y + b) + ")\n";
    return res;
}



void ref_func_add_10( int& a, std::string& b )
{
    a += 10;
    
    static int count = 0;
    if( count % 2 == 0 )
        std::transform(b.begin(), b.end(), b.begin(), ::toupper );
    else
        std::transform(b.begin(), b.end(), b.begin(), ::tolower );
    
    count++;
}





void test_bind()
{
    std::function<int(int,char,std::string,double)> func1;
    
    if( func1 == nullptr )
        cout << "func not bind\n";
    
    func1 = std::bind( target_func, std::placeholders::_1, 
                                   std::placeholders::_2,
                                   std::placeholders::_3,
                                   std::placeholders::_4 );
    
    if( func1 != nullptr )
        cout << "run func = " << func1( 14, 'Z', "this is a dog.", 451.0551143 ) << endl;
    
    //
    TargetObj obj;
    function<string(int,int)> func2 = std::bind( &TargetObj::obj_func, &obj, 
                                                 std::placeholders::_1, std::placeholders::_2 );
    
    cout << func2(-551,1) << endl;
    
    //
    using namespace std::placeholders;
    //std::function<void(int&,string&)> func3;
    auto func3 = std::bind( ref_func_add_10, _1, _2 );
    
    int a = 100;
    string b = "Aa Bb Cc Dd Ee Ff Gg";
    
    // mac執行結果,會出現ref的效果.
    func3( a, b );
    cout << "a = " << a << ", b = " << b << endl; 
    
    // 看講法是使用thread的時候如果引數是reference,有機會需要用ref()來傳. 有空實驗看看
    auto func4 = std::bind( ref_func_add_10, ref(a), ref(b) );
    
    func4();
    cout << "a = " << a << ", b = " << b << endl;
    
    //
    auto func5 = std::mem_fn( &TargetObj::obj_func );
    cout << func5( obj, 0, 0 ) << endl;
}





void test_functions()
{
    test_less_func();
    
    test_bind();
}





void test_pair()
{
    auto p1 = std::make_pair( "John", 12345 );
    cout << "p1.first = " << p1.first << ", p1.second = " << p1.second << endl;
    
    //
    auto pair_func = [] ( string str, int x, int y ) -> pair<string,double> {
        double d = 1.0 * x / y;
        std::string result = std::to_string( d ) + " " + str;
        return pair<string,double>{ result, d };
    };
    
    pair<string,double> p2 = pair_func( "Andy", 551, 423 );
    cout << "p2.first = " << p2.first << ", p2.second = " << p2.second << endl; 
}





std::ostream& operator << ( std::ostream& os, std::vector<double> c )
{
    for( auto itr : c )
        os << itr << " ";
    return os;
}



std::ostream& operator << ( std::ostream& os, std::list<int> c )
{
    for( auto itr : c )
        os << itr << " ";
    return os;
}






void test_tuple()
{
    //
    std::tuple<int,int,char> t1 = std::make_tuple( 10, -5, 'Z' );
    int     a1 = std::get<0>(t1),
            b1 = std::get<1>(t1);
    char    c1 = std::get<2>(t1);
    
    cout << "a1 = " << a1 << ", b1 = " << b1 << ", c1 = " << c1 << endl;
    
    //
    auto t2 = std::make_tuple( 0, 0.0 );
    std::get<0>(t2) = 100;
    std::get<1>(t2) = -5.11;
    cout << "a2 = " << std::get<0>(t2) << ", b2 = " << std::get<1>(t2) << endl;
    
    //
    auto t3 = make_tuple( string("This is a dog."), 
                          vector<double>{1.1, 2.2, 3.3, 4.4},
                          list<int>{ -1, -2, -3 } );
    
    string s3;
    vector<double> v3;
    list<int> list4;
    
    // tie的作用相當於用reference的方式傳入資料, assign後面tuple的值進去.
    // 這樣不必用get一個一個取出.
    std::tie( s3, v3, list4 ) = t3; 
    // std::tie( s3, ignore, l4 ) = t3;  // 可以用ignore的方式避免全部資料都需要assign
    cout << s3 << endl << v3 << endl << list4 << endl;
    
    std::tie( s3, v3, list4 ) = std::make_tuple( string("This is a cat."), 
                                                 vector<double>{-1.1, -2.2, -3.3, -4.4},
                                                 list<int>{ 1, 2, 3 } );
    
    // 這個語法也能work
    //std::tie( s3, v3, list4 ) = tuple<string,vector<double>,list<int>>( string("This is a cat."), 
      //                                           vector<double>{-1.1, -2.2, -3.3, -4.4},
        //                                         list<int>{ 1, 2, 3 } );
                                

    cout << s3 << endl << v3 << endl << list4 << endl;
    
    // 
    tuple<int,double,string> t4 = { 1, 1.1, "How are you" };
    cout << "a4 = " << get<0>(t4) << ", b4 = " << get<1>(t4) << ", c4 = " << get<2>(t4) << endl;
    
    int a, b;
    //std::tie(a,b) = {10,10}; // 編譯錯
    std::tie( a, b ) = std::make_tuple( 1, 2 );
    cout << "a = " << a << ", b = " << b << endl;
    
    //
    auto func = []() -> tuple<int,int> {
        return make_tuple(4,-1);
    };
    
    // 比較實用的做法, 直接用tie接收函數回傳值
    tie( a, b ) = func();
    cout << "a = " << a << ", b = " << b << endl;
}
