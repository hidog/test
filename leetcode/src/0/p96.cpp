#include "p96.h"


class Solution2 {
public:
    int numTrees(int n) {
        int table[20] = {0};
        table[0] = 1;
        table[1] = 1;
        
        int left, right, sum;
        for( int idx = 2; idx <= n; idx++ )
        {
            sum = 0;
            for( int i = 1; i <= idx; i++ )
            {
                left = table[i-1];
                right = table[idx-i];
                sum += left * right;
            }
            table[idx] = sum;
        }
        return table[n];
    }
};


void    p96()
{
    Solution2 so;
    cout << so.numTrees(3) << endl;
}
