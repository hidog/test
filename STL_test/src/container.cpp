#include "container.h"
#include <cstdlib>
#include <random>

#include <vector>
#include <iostream>
#include <algorithm>
#include <deque>
#include <array>
#include <list>
#include <forward_list>
#include <bitset>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>

#include "my_array.h"
#include "my_map_class.hpp"


using namespace std;

template<typename value>
using MapInt = map<int,value>;

using MapStr = map<string,string>;






void test_map_1()
{
    MapInt<double> m;
    
    auto item = m[10];
    cout << "use undefined item " << item << endl;  // value = 0. use as default value.
    
    m.clear();
    
    // p2 will insert fail. key repeat.
    pair<MapInt<double>::iterator, bool> p1 = m.insert( {1, 1.134} );
    auto p2 = m.insert( { 1, 23.441 } );

    if( p1.second == true )
        cout << "p1 insert success. value = " << p1.first->first << ", " << p1.first->second << endl;
    else
        cout << "p1 insert fail." << endl;
        
    if( p2.second == true )
        cout << "p2 insert success. value = " << p2.first->first << ", " << p2.first->second << endl;
    else
        cout << "p2 insert fail." << endl;
        
    // 
    pair<MapInt<double>::iterator, bool> p3 = m.emplace( 100, -0.155133 );
    auto p4 = m.emplace( 100, 55151 );
    
    if( p3.second == true )
        cout << "p3 insert success. value = " << p3.first->first << ", " << p3.first->second << endl;
    else
        cout << "p3 insert fail." << endl;
        
    if( p4.second == true )
        cout << "p4 emplace success. value = " << p4.first->first << ", " << p4.first->second << endl;
    else
        cout << "p4 emplace fail." << endl;

    // 一些舊的語法
    m.insert( pair<int,double>(-1,-51.23) );
    m.insert( make_pair(4141,5555.1111) );

    cout << "m[-1] = " << m[-1] << endl
         << "m[4141] = " << m[4141] << endl;
}




void test_map_2()
{
    map<string, string, MyLess> m;
    //map<string, string, std::less<> > m;
    
    m["John"] = "123";
    m["Sally"] = "456";
    m["Alice"] = "This is dog";
    m["Andy"] = "My god";
    
    for( auto itr : m )
        cout << itr.first << " " << itr.second << endl;
    cout << endl;
        
    auto itr =  m.find("John");
    cout << itr->first << " " << itr->second << endl;
        
    auto itr2 = m.find("Peter");
    if( itr2 == m.end() )
        cout << "not found.\n";     
        
    // 
    auto itr3 = m.upper_bound("A");
    cout << itr3->first << ", " << itr3->second << endl;
    
    auto itr4 = m.lower_bound("A");
    if( itr4 == m.end() )
        cout << "out of lower bdd\n";   
}





void test_multi_map()
{
    using multi_int_str = multimap<int,string>;
    
    multi_int_str m;
    
    m.emplace( 1, "A" );
    m.emplace( 1, "B" );
    m.emplace( 1, "C" );
    m.emplace( 1, "AAAA" );
    m.emplace( 1, "BBBB" );
    m.emplace( 1, "CCCC" );
    m.emplace( 2, "D" );
    m.emplace( 3, "E" );
    
    std::pair<multi_int_str::iterator,multi_int_str::iterator> m2 = m.equal_range(1);
    
    // m2' first = begin, second = end
    for( auto itr = m2.first; itr != m2.second; ++itr )
        cout << itr->first << " " << itr->second << endl; 
    cout << endl;
}






void test_unordered_map_1()
{
    map<int,int> m1;
    unordered_map<int,int> m2;
    
    constexpr int size = 30;
    array<int,size> a;
    for( auto &item : a )
        item    =   rand() % 1000;
        
    for( const auto& item : a )
    {
        m1[item]    =   item;
        m2[item]    =   item;
    }
    
    cout << "m1 is ordered map, m2 is unordered map\n";
    auto itr1 = m1.begin();
    auto itr2 = m2.begin();
    for( ; itr1 != m1.end(); ++itr1, ++itr2 )    
        cout << "m1[" << itr1->first << "], m2[" << itr2->first << "]\n";
    cout << endl;
}








void test_unordered_map_2()
{
    unordered_map<int, int, MyHash<101>, MyHash<101>> m;
    
    m.emplace( 523415,  0 );
    m.emplace( 1234,    1 );
    m.emplace( 5151,    2 );
    m.emplace( 123,     3 );
    m.emplace( 9876451, 4 );
    m.emplace( 103,     5 );
    m.emplace( 204,     6 );
    
    cout << endl << "size = " << m.size() << endl; // size = 5, 有兩個物件重疊. (執行結果跟預期的有點不同)
    for( auto itr : m )
        cout << itr.first << " " << itr.second << endl;
    cout << endl;
}





void test_map_3()
{
    map<string,int> m = { {"a", 10}, {"b", 555}, {"c", 666} };
    /* 
        可以使用function object. 
        GreaterThan是一個物件,但使用上可以像是一個函數
        GreaterThan greater_than(100);    
        pair<string,int> p = { "aaa", 100 };
        greater_than(p);
    */
    GreaterThan greater_than(555);    
    pair<string,int> p = { "aaa", 777 };
    bool flag = greater_than(p);
    if( flag == true )
        cout << "true!!\n";

    auto v = find_if( m.begin(), m.end(), GreaterThan{199} ); 
    cout << v->first << " " << v->second << endl;

    auto v2 = find_if( m.begin(), m.end(), greater_than ); 
    cout << v2->first << " " << v2->second << endl;
}





void test_set()
{
    int     i,  r;
    set<int>    si{1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    for( i = 0; i < 10; i++ )
    {
        pair<set<int>::iterator, bool> itr = si.insert( rand() % 100 );
        if( itr.second == false )
            cout << "insert " << *itr.first << " fail.\n";

    }
    for( i = 0; i < 10; i++ )
    {
        pair<set<int>::iterator, bool> itr = si.emplace( rand() % 100 );
        if( itr.second == false )
            cout << "insert " << *itr.first << " fail.\n";
    }

    cout << "view set : \n";
    for_each( si.begin(), si.end(), []( int a ){
        cout << a << " ";
    } );
    cout << endl;

    //
    for( i = 0; i < 10; i++ )
    {
        r   =   rand() % 20;
        auto itr = si.find(r);
        if( itr == si.end() )
            cout << "set not found " << r << endl;
        else
            cout << "found " << r << endl;
    }

    //
    auto itr = si.begin();
    while( itr != si.end() )
    {
        if( *itr % 3 == 1 )
            itr = si.erase(itr);
        else
            ++itr;
    }

    cout << "\nafter erase 3n+1...\n";
    for( auto itr : si )
        cout << itr << " ";
    cout << endl;

}






void test_container_func_3()
{
    test_map_1();
    cout << endl;    
    test_map_2();
    cout << endl;
    test_map_3();
    cout << endl;
    
    test_multi_map();
    cout << endl;
    
    test_unordered_map_1();
    test_unordered_map_2();

    test_set();
}






void test_bitset_2()
{
    std::bitset<4> b1{0}, b2{42};
    std::bitset<8> b3{0}, b4{42};
 
    std::hash<std::bitset<4>> hash_fn4;
    std::hash<std::bitset<8>> hash_fn8;
    using bin64 = std::bitset<64>;
 
    // windows, mac執行結果不同. 
    // win執行結果如預期, mac跟預期的不同.
    // linux執行結果也正常.
    cout << bin64{hash_fn4(b1)} << " = " << hash_fn4(b1) << '\n'
         << bin64{hash_fn4(b2)} << " = " << hash_fn4(b2) << '\n'
         << bin64{hash_fn8(b3)} << " = " << hash_fn8(b3) << '\n'
         << bin64{hash_fn8(b4)} << " = " << hash_fn8(b4) << '\n';
}





void test_bitset_1()
{
    std::bitset<5> b1 = 0B11010;   // 0B means bit.    value = 11010.
    std::bitset<4> b2{0xA};
    std::bitset<7> b3("0011010");  // can't be constexpr yet
    // 0B01111010. 8=長度,C表示C置換成0,W表示W置換成1
    std::bitset<8> b4 = std::bitset<8>( "CWWWWCWC", 8, 'C', 'W'); 
    
    cout << "b4 = " << b4 << ", b4[3] = " << (b4[3] ? "true" : "false") << "\n\n";
    
    // 
    cout << "b1 = " << b1 << "\nb2 = " << b2 << "\nb3 = " << b3 << "\n\n";
 
    //
    b1 |= 0b11111; 
    b2 &= 0b0000; 
    b3 ^= std::bitset<7>{0b1111111}; 
    
    cout << "b1 = " << b1 << "\nb2 = " << b2 << "\nb3 = " << b3 << "\n\n";
 
    //
    b1.reset();  // set to 0
    b2.set();    // set to 1
    b3.flip();   // 0變1, 1變0
    cout << "b1 = " << b1 << "\nb2 = " << b2 << "\nb3 = " << b3 << "\n";
    cout << (b1.all() ? "yes" : "no") << endl
         << (b2.none() ? "yes" : "no") << endl
         << (b3.any() ? "yes" : "no") << endl << endl;
 
    //
    b2.set( 0, false ); 
    b2.set( 1, true ); 
    b2.flip( 2 ); 
    b2.reset( 3 );
    cout << "b2 = " << b2 << "\n";
    
    //
    b3[2] = true;
    cout << "b3 = " << b3 << "\n";
 
    // 
    cout << "count = " << b3.count() << endl
         << "size = " << b3.size() << endl
         << "unsigned long long = " << b2.to_ullong() << endl
         << "string = " << b2.to_string() << "\n\n";
}





void test_stack()
{
    stack<int> si;
    for( int i = 0; i < 10; i++ )
        si.push(i);
    for( int i = 0; i < 10; i++ )
        si.emplace(i + 100);
        
    cout << "top = " << si.top() << endl;
    
    while( si.empty() == false )
    {
        auto item = si.top();
        cout << item << " ";
        si.pop();
    }
    cout << endl;
}




void test_queue()
{
    queue<int> qu;
    for( int i = 0; i < 10; i++ )
        qu.push(i);
    for( int i = 0; i < 10; i++ )
        qu.emplace(i + 100);       
    
    while( qu.empty() == false )
    {
        cout << "front = " << qu.front() << " "
             << "back = " << qu.back() << endl;
        qu.pop();
    }
    cout << endl;
}




void test_piority_queue()
{
    class Cmp
    {
    public:
        bool operator () ( int left, int right )
        {
            return left < right;
        }
    };

    priority_queue<int, deque<int>, Cmp> pq;
    
    for( int i = 0; i < 10; i++ )
        pq.emplace( rand()%1000 );
    for( int i = 0; i < 10; i++ )
        pq.push( rand()%1000 + 3000 );
        
    while( pq.empty() == false )
    {
        auto item = pq.top();
        cout << "top = " << item << "\n";
        pq.pop();
    }
    cout << endl << endl; 
}




               





void test_container_func_2()
{
    test_bitset_1();
    test_bitset_2();
    cout << endl;
    
    test_stack();
    test_queue();
    cout << endl;
    
    test_piority_queue();
}





void test_vector_emplace_1()
{
    vector<MyArray> vec0, vec1, vec2;
    
    //
    MyArray m0;
    vec0.push_back(m0);
    m0.print();
    vec0[0].print();
    cout << endl;
    
    //
    MyArray m1;
    vec0.emplace_back(m1);
    m1.print();
    vec0[1].print();
    cout << endl;
    
    //
    MyArray m2;
    vec1.push_back( std::move(m2) );
    m2.print();
    vec1[0].print();
    cout << endl;
    
    // 
    MyArray m3;
    vec2.emplace_back( std::move(m3) );
    m3.print();
    vec2[0].print();
    cout << endl;
}





void test_vector_emplace_2()
{
    reset_shift();
    
    std::vector<MyArray> vec1;
    
    /* 
     在使用emplace back的時候有機會呼叫resize,
     需要資料搬移,造成呼叫大量的copy constructor.
     第一次跟之後呼叫emplace_back的執行效率不同.
     要留意resize造成的效能影響.
    */
    // emplace可以直接呼叫MyArray的constructor. 這點跟push_back不同
    vec1.emplace_back( "m0 str1", "m0 str2" ); 
    vec1[0].print();
    cout << endl;
    
    vec1.emplace_back( MyArray("m1 str1", "m1 str2") ); // 也可以用傳統方式呼叫
    vec1.emplace_back( "m2 str1", "m2 str2" );
    vec1.emplace_back( "m3 str1", "m3 str2" );
    vec1[3].print();
    cout << endl;
    
    // 
    reset_shift();
    std::vector<MyArray> vec2;
    // 使用reserve可以避免resize造成呼叫copy constructor.
    // 如果需要大量插入刪除,比較好的做法是改成list
    vec2.reserve(5); 
    cout << endl;

    vec2.emplace_back( "m0 str1", "m0 str2" );
    vec2.emplace_back( "m1 str1", "m1 str2" );
    vec2.emplace_back( "m2 str1", "m2 str2" );
    vec2[2].print();
    cout << endl;
}




void test_vector_emplace_3()
{
    reset_shift();
    vector<MyArray> vec0, vec1, vec2;
    
    //vec0.push_back( "m5 str1", "m5 str2" ); // 不支援這個寫法,跟emplace的差異之一
    vec0.push_back( MyArray("m0 str1","m0 str2") );
    vec0[0].print();
    cout << endl;

    //
    MyArray a, b;
    cout << endl; 
    
    // 實際上已經有 push_pack的右值版本了.
    vec1.push_back( a+b );
    vec1[0].print();
    cout << endl;
    
    // 
    vec2.emplace_back( a+b );
    vec2[0].print();
    cout << endl;
}





void test_vector_alg()
{
    vector<int> vec { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int shift = 1;
    
    size_t count = std::count_if( vec.begin(), 
                                  vec.end(), 
                                  [=] (int a) -> bool {
                                        return (a+shift)%2 == 0;
                                  } );
    
    cout << "count = " << count << endl << endl;
    
    // 
    vector<int> vec1(10), vec2(10);
    for( int i = 0; i < 10; i++ )
    {
        vec1[i] =   rand() % 10 + 100;
        vec2[i] =   rand() % 10 + 200;
    }
    
    cout << "vec1 = ";
    for( auto itr : vec1 )
        cout << itr << " ";
    cout << endl;
    
    cout << "vec2 = ";
    for( auto itr : vec2 )
        cout << itr << " ";
    cout << endl;
    
    std::swap(vec1, vec2);
    cout << "\nafter swap\n";
    
    cout << "vec1 = ";
    for( auto itr : vec1 )
        cout << itr << " ";
    cout << endl;
    
    cout << "vec2 = ";
    for( auto itr : vec2 )
        cout << itr << " ";
    cout << endl;
    
    //
    for( int i = 0; i < 10; i++ )
        vec[i]  =   rand() % 10;
    std::sort( vec.begin(), vec.end() );
    
    cout << "\nafter sort\n";
    for( auto itr : vec )
        cout << itr << " ";
    cout << endl;
    
    // 不會真的移除物件,需要手動取得end位置.
    std::vector<int>::iterator vend = std::unique( vec.begin(), vec.end() );
    for_each( vec.begin(), vend, [](int a){
        cout << a << " ";
    } );
    cout << endl;
    
    //
    auto output = [] ( const vector<int>& v ) {
        for( const auto& itr : v )
            cout << itr << " ";
    };
    

#ifdef _WIN32 
    // vs2019,沒先排序,會跳錯誤
    sort(vec1.begin(), vec1.end());
    sort(vec2.begin(), vec2.end());
#endif
    std::merge( vec1.begin(), vec1.end(), vec2.begin(), vec2.end(),  std::back_inserter(vec) );
    //std::merge( vec1.begin(), vec1.end(), vec2.begin(), vec2.end(),  std::front_inserter(vec) ); // not support.
    cout << "\nafter merge\n";

    cout << "vec = ";
    output(vec);
    // vec1, vec2資料仍在,未消失.
    cout << "\nvec1 = ";
    output(vec1);
    cout << "\nvec2 = ";
    output(vec2);
    cout << endl;
    
    //
    std::reverse( vec.begin(), vec.end() );
    cout << "\nafter reverse\n";
    for( auto itr : vec )
        cout << itr << " ";
    cout << endl;
    
    //
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle( vec.begin(), vec.end(), g ); // c++11 not support random_shuffle
    
    cout << "\nafter shuffle\n";
    for( auto itr : vec )
        cout << itr << " ";
    cout << endl;
    
    cout << "\nafter qsort\n";
    std::qsort( vec.data(), vec.size(), sizeof(int),
               []( const void* x, const void* y ) {
                   const int arg1 = *static_cast<const int*>(x);
                   const int arg2 = *static_cast<const int*>(y);
                   if( arg1 < arg2 ) 
                       return -1;
                   else if( arg1 > arg2 ) 
                       return 1;
                   else
                       return 0;
               });
    for( auto itr : vec )
        cout << itr << " ";
    cout << endl;            

                           
}






void test_deque_alg()
{
    //
    deque<double> cq = { 0, 2.3, 4, -11, -12.3, 55, 34, 9, 45.113 };
    cq.emplace_back( 1000 );
    cq.emplace_back( -111 );
    cq.emplace_front( 5 );
    cq.emplace_front( 7 );
    
    sort( cq.begin(), cq.end(), 
            [](double a, double b) {
                return a > b; // 從大到小排
            } );
    
    for( auto item : cq )
        cout << item << ", ";
    cout << endl;
    
    cout << cq[3] << endl;
}




void test_array_alg()
{
    std::array<int, 100> a = { 98, 1, 2, 3, 5, 4, 3, 2, 1, 0, -1, -2, 2, 2, 3, 3, 4, 5, 6, 10, 100, 99, -110, 22 };
    
    std::qsort( a.data(), a.size(),
                sizeof(decltype(a)::value_type),
                []( const void* x, const void* y ) {
                    const int arg1 = *static_cast<const int*>(x);
                    const int arg2 = *static_cast<const int*>(y);
                    if( arg1 < arg2 ) 
                        return 1;
                    else if( arg1 > arg2 ) 
                        return -1;
                    else
                        return 0;
                });
 
    for (int item : a)
        cout << item << ' ';
    cout << endl;
}





void test_list_alg()
{
    list<int> c = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
    c.emplace_front( 100 );
    c.emplace_back( 200 );
    auto itr = c.begin();
    itr++; itr++; itr++;  // 純粹測試語法,會在第三個位置插入99
    c.emplace( itr, -99 ); 
    
    for( auto item : c )
        cout << item << " ";
    cout << endl << endl;
    
    //
    std::replace_if( c.begin(), c.end(), [](int a) -> bool {
            int c = 1;
            while(true)
            {
                a   -=  c;
                if( a <= 0 )
                    break;
                c++;
            }
            if( a == 0 )
                return true;
            else
                return false;
        }, 9999 );
    
    for( auto item : c )
        cout << item << " ";
    cout << endl << endl;
    
    //
    list<int> c1, c2;
    for( int i = 0; i < 10; i++ )
    {
        c1.emplace_back( rand() % 100 );
        c2.emplace_back( rand() % 100 );
    }
    cout << "\nview list\n";
    cout << "c1 = ";
    for( auto itr : c1 )
        cout << itr << " ";
    cout << endl;
    cout << "c2 = ";
    for( auto itr : c2 )
        cout << itr << " ";
    cout << endl;
    
    // 
    //c1.splice( c1.begin(), c2 ); // 接在前面
    c1.splice( c1.end(), c2 ); // 接在後面

    cout << "\nafter spliace\n";
    cout << "c1 = ";
    for( auto itr : c1 )
        cout << itr << " ";
    cout << endl;
    
    // c2經過splice後資料會被清空
    cout << "c2 = ";
    for( auto itr : c2 )
        cout << itr << " ";
    cout << endl;
}



void test_forward_list_alg()
{
    forward_list<int> c = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    
    // remove並沒有真的移除物件,如果沒重新取得end,會列印出尾端的資料.
    auto end = remove_if( c.begin(), c.end(), 
               [](int a) -> bool { 
                    return a % 3 == 1; 
               } );
    
    for_each( c.begin(), end, 
              [](int a) { 
                    cout << a << " "; 
              } );
}



void test_container_func_1()
{
    // vector
    test_vector_emplace_1();
    cout << endl;
    
    test_vector_emplace_2();
    cout << endl;
    
    test_vector_emplace_3();
    cout << endl;
    
    test_vector_alg();
    
    // deque
    test_deque_alg();
    
    // array
    test_array_alg();
    
    // list
    test_list_alg();
    
    // forward list
    test_forward_list_alg();
}



