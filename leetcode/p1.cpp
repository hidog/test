#include "p1.h"

#include <vector>
#include <iostream>
#include <cstring>

using namespace std;



vector<int> twoSum(vector<int>& nums, int target)
{
    vector<int> res(2);
    for( int i = 0; i < nums.size(); i++ )
    {
        for( int j = i+1; j < nums.size(); j++ )
        {
            if( nums[i] + nums[j] == target )
            {
                res[0] = i;
                res[1] = j;
                break;              
            }        
        }    
    }
    return res;
}



void    p1_two_sum()
{
    vector<int> nums { 2, 7, 11, 15 };
    vector<int> res = twoSum( nums, 9 );
    cout << res[0] << ", " << res[1] << endl;
}






 /* Definition for singly-linked list.*/
 struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
 };
 

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) 
{
    int carry = 0;
    int value = 0;
    ListNode *result = new ListNode;
    ListNode *res_head = result;
    
    while(true)
    {            
        value = result->val;
        if( l1 != nullptr )
            value += l1->val;
        if( l2 != nullptr )
            value += l2->val;
    
        if( value >= 10 )
            carry = 1;
        else
            carry = 0;
        value %= 10;

        result->val = value;           
        
        if( l1 != nullptr )
            l1 = l1->next;
        if( l2 != nullptr )
            l2 = l2->next;
        
        if( l1 == nullptr && l2 == nullptr )
        {
            if( carry > 0 )
                result->next = new ListNode(carry);
            break;
        }
        else            
            result->next = new ListNode(carry);
        result = result->next;
    }
    
    return res_head;    
}




void    p2_Add_Two_Numbers()
{
#if 1
    ListNode* l1 = new ListNode(9);
    ListNode* start_l1 = l1;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9); 
    l1 = l1->next;
    l1->next = new ListNode(9);
    ListNode* l2 = new ListNode(9);
    ListNode* start_l2 = l2;
    l2->next = new ListNode(9); 
    l2 = l2->next;
    l2->next = new ListNode(9); 
#else
    ListNode* l1 = new ListNode(2);
    ListNode* start_l1 = l1;
    l1->next = new ListNode(4); 
    l1 = l1->next;
    l1->next = new ListNode(3); 

    ListNode* l2 = new ListNode(5);
    ListNode* start_l2 = l2;
    l2->next = new ListNode(6); 
    l2 = l2->next;
    l2->next = new ListNode(4); 
#endif
    ListNode* res = addTwoNumbers(start_l1,start_l2);
    while( res != nullptr )
    {
        cout << res->val;
        res = res->next;
    }
    cout << endl;
}





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





double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) 
{
    int mn = nums1.size() + nums2.size();
    int count = 0;
    int n1 = 0, n2 = 0;
    vector<int> a;
    
    while(true)
    {
        if( n1 >= nums1.size() && n2 >= nums2.size() )
            break;
        else if( n2 >= nums2.size() )
        {
            a.push_back( nums1[n1] );
            n1++;
        }
        else if( n1 >= nums1.size() )
        {
            a.push_back( nums2[n2] );
            n2++;
        }
        else if( nums1[n1] < nums2[n2] )
        {
            a.push_back( nums1[n1] );
            n1++;
        }
        else
        {
            a.push_back( nums2[n2] );
            n2++;
        }
    }
    
    if( a.size() == 0 )
        return 0;
    else if( a.size() % 2 == 1 )
        return a[ a.size()/2 ];
    else
        return 0.5*(a[a.size()/2] + a[a.size()/2-1]);
}


void    p4_Median_of_Two_Sorted_Arrays()
{
    vector<int> a{3,4,5};
    vector<int> b{2,5,6};
    cout << findMedianSortedArrays( a, b );
}






bool isPalindrome(int x) 
{        
    if( x < 0 )
        return false;
    
    int64_t y = x, z = 0;
    while( y > 0 )
    {
        z = z*10 + y%10;
        y /= 10;            
    }
    if( z == x )
        return true;
    else
        return false;
}

void    p9_PalindromeNumber()
{
    cout << isPalindrome(2147483647) ? "yes" : "no";
}





 struct TreeNode {
     int val;
     TreeNode *left;
     TreeNode *right;
     TreeNode() : val(0), left(nullptr), right(nullptr) {}
     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 };
 
class Solution {
public:
    vector<TreeNode*> generateTrees(int n) 
    {
        vector<int> vec;
        vec.resize(n);
        for( int i = 1; i <= n; i++ )
            vec[i-1] = i;
        return get_tree(vec);
    }
    
    vector<TreeNode*> get_tree( vector<int> vec )
    {
        int n = vec.size();
        
        if( n == 1 )
        {
            vector<TreeNode*> result;
            TreeNode* node = new TreeNode(vec[0], nullptr, nullptr );
            result.push_back(node);
            return result;
        }
        else
        {
            vector<TreeNode*> result;
            {
                // case first
                vector<int> subvec;
                for( int i = 1; i < vec.size(); i++ )
                    subvec.push_back(vec[i]);
                vector<TreeNode*> right = get_tree(subvec);                
                for( int i = 0; i < right.size(); i++ )                
                    result.emplace_back( new TreeNode(vec[0], nullptr, right[i]) );
            }
            {
                vector<int> leftvec, rightvec;
                for( int idx = 1; idx <= vec.size()-2; idx++ )
                {
                    leftvec.clear();
                    rightvec.clear();
                    for( int x = 0; x < idx; x++ )
                        leftvec.push_back(vec[x]);
                    for( int x = idx+1; x < vec.size(); x++ )
                        rightvec.push_back(vec[x]);
                    
                    vector<TreeNode*> left = get_tree(leftvec);
                    vector<TreeNode*> right = get_tree(rightvec);
                    
                    for( int i = 0; i < left.size(); i++ )
                    {
                        for( int j = 0; j < right.size(); j++ )                        
                            result.emplace_back( new TreeNode(vec[idx], left[i], right[j]) );
                    }
                }
            }
            {
                // case last
                vector<int> subvec;
                for( int i = 0; i < vec.size()-1; i++ )
                    subvec.push_back(vec[i]);
                vector<TreeNode*> left = get_tree(subvec);
                for( int i = 0; i < left.size(); i++ )                
                    result.emplace_back( new TreeNode(vec[n-1], left[i], nullptr) );
            }
            return result;
        }
    }
};


void print_tree( TreeNode* node )
{
    if( node == nullptr )
    {
        cout << "null,";
        return;
    }
    cout << node->val << ",";
    print_tree(node->left);
    print_tree(node->right);
}


void    p95()
{
    Solution so;
    vector<TreeNode*> tree = so.generateTrees(3);
    cout << "start" << endl;
    for( int i = 0; i < tree.size(); i++ )
    {
        print_tree(tree[i]);
        cout << endl;
    }
    cout << "end" << endl;
}




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

