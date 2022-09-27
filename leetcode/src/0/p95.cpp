#include "p95.h"


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


void    p95_Unique_Binary_Search_Trees_II()
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

