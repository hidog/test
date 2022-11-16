#include "p1007.h"

#include <vector>
#include <iostream>

using namespace std;





int minDominoRotations(vector<int>& tops, vector<int>& bottoms) 
{
    int a[2] = { tops[0], bottoms[0] };
    int i;
    int size = tops.size();
    
    for( i = 1; i < size; i++ )
    {
        if( a[0] != tops[i] && a[0] != bottoms[i] )
            a[0] = -1;
        if( a[1] != tops[i] && a[1] != bottoms[i] )
            a[1] = -1;
        if( a[0] == -1 && a[1] == -1 )
            break;
    }
    
    if( a[0] == -1 && a[1] == -1 )
        return -1;
        
    int count_1 = 0, count_2 = 0;
    if( a[0] != -1 )
    {
        for( i = 0; i < size; i++ )
        {
            if( a[0] == tops[i] )
                count_1++;
            if( a[0] == bottoms[i] )
                count_2++;
        }
    }
    else
    {
        for( i = 0; i < size; i++ )
        {
            if( a[1] == tops[i] )
                count_1++;
            if( a[1] == bottoms[i] )
                count_2++;
        }
    }
    return count_1 > count_2 ? size - count_1 : size - count_2;
}


void p1007_Minimum_Domino_Rotations_For_Equal_Row()
{
    //vector<int> tops { 3, 5, 1, 2, 3 };
    //vector<int> bottoms { 3, 6, 3,3, 4 };
    
    vector<int> tops { 2, 1, 2, 4, 2, 2 };
    vector<int> bottoms { 5, 2, 6, 2, 3, 2 };
    
    int res = minDominoRotations( tops, bottoms );
    cout << res << endl;

}
