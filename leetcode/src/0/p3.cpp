#include "p3.h"


int lengthOfLongestSubstring(string s) 
{
    if( s.size() == 0 )
        return 0;
    
    int start = 0;
    int end = 0;
    int table[256] = {0};
    
    int max = 0;
    
    while(true)
    {
        if( table[ s[end] ] == 1 )        
        {
            if( start == end )
            {
                if( max < end - start + 1 )
                    max = end - start + 1;                
                break;
            }
            table[ s[start] ] = 0;
            start++;
        }
        else
        {            
            if( max < end - start + 1 )
                max = end - start + 1;
            table[ s[end] ] = 1;
            end++;
            if( end >= s.size() )
                break;
        }
    }
    
    return max;
}




void    p3_Longest_Substring_Without_Repeating_Characters()
{
    string s = "";
    cout << lengthOfLongestSubstring(s);
}




