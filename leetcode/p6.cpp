#include "p6.h"

#include <iostream>

using namespace std;




/**
 * Definition for a binary tree node.
 */
struct TreeNode 
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};


class Solution {
public:
    TreeNode* trimBST(TreeNode* root, int low, int high) 
    {
        if( root == nullptr )
            return root;
        else if( root->val < low )
        {
            root = root->right;
            trimBST( root, low, high );
        }
        else if( root->val > high )
        {
            root = root->left;
            trimBST( root, low, high );
        }
    }

    void output( TreeNode* root )
    {
        if( root != nullptr )
            cout << root->val << " ";
        else
            cout << "null ";
        
        output( root->left );
        output( root->right );
    }
};




void p669_Trim_a_Binary_Search_Tree()
{
    TreeNode n1( 1 );
    TreeNode n2( 2, &n1, nullptr );
    TreeNode n3( 0, nullptr, &n2 );
    TreeNode n4( 4 );
    TreeNode n5( 3, &n3, &n4 );

    Solution so;
    TreeNode *result = so.trimBST( &n5, 1, 3 );

    so.output( result );
}