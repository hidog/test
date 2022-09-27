#include "p2.h"


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
