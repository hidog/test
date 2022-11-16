#include <iostream>
#include <string>
#include <iomanip>
#include <cassert>

using namespace std;

int main()
{
    int i, n;
    int count = 0;
    int index_1 = 0, index_2 = 0;
    cin >> n;
    string str;
    str.resize(n);

    // init
    for( i = 0; i < n; i++ )
        str[i] = 'a' + i;

    // start
    while(true)
    {
        cout << setw(8) << count++ << ": " << str << endl;       
        //if( count == 2 )
         //   printf("test");

        for( i = n-2; i >= 0; i-- )
        {
            if( str[i+1] > str[i] )
            {
                index_1 = i;
                break;
            }
        }
        if( i < 0 )
            break;

        for( i = n-1; i > index_1; i-- )
        {
            if( str[i] > str[index_1] )
            {
                index_2 = i;
                break;
            }
        }

        swap( str[index_1], str[index_2] );

        for( i = index_1+1; i < (index_1+n+1)/2; i++ )
            swap( str[i], str[n-i+index_1] );
    }

    int checksum = 1;
    for( i = 2; i <= n; i++ )
        checksum *= i;
    cout << "n! = " << checksum << endl;

    return 0;
}

