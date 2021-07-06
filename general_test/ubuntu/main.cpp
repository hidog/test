#include <iostream>
#include "src/linklist.h"

using namespace std;


int main()
{
    LinkList	list;

    cout << list.size() << endl;
    list.print();

    list.push_back(444);
    list.print();

    list.insert( 0, 3 );
    list.insert( 0, 32 );
    list.insert( 1, 35 );
    for( int i = 0; i < 100; i++ )
        list.insert( i, i + 55 );

    list.print();


    cout << "size = " << list.size() << endl;

    LinkList	list2(list);
    list2.print();

    system("PAUSE");
    return 0;
}

