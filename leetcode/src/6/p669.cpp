#include "p669.h"

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
            root = trimBST( root->right, low, high );
        else if( root->val > high )        
            root = trimBST( root->left, low, high );        
        else if( root->val >= low && root->val <= high )
        {
            root->left = trimBST( root->left, low, high );
            root->right = trimBST( root->right, low, high );
        }

        return root;
    }

    void output( TreeNode* root )
    {
        if( root != nullptr )
            cout << root->val << " ";
        else
        {
            cout << "null ";
            return;
        }
        
        output( root->left );
        output( root->right );
    }
};




void p669_Trim_a_Binary_Search_Tree()
{
    TreeNode n1( 2 );
    TreeNode n2( 1, nullptr, &n1 );
    TreeNode n3( 4 );
    TreeNode n4( 3, &n2, &n3 );

    Solution so;
    so.output(&n4);
    cout << "\n";

    TreeNode *result = so.trimBST( &n4, 3, 4 );

    so.output( result );
    cout << "\n";

}
