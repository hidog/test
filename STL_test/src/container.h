#ifndef CONTAINER_H
#define CONTAINER_H

/*
 https://en.cppreference.com/w/cpp/memory/allocator
 allocator, 一些進階用法, 覺得用不太到就沒繼續看了.
 */


/* 
note:   emplace的傳入參數是右值參考,配合move可以得到比較好的效率.
        實測結果比較新的環境已經有push_back的右值版本,所以理論上emplace_back, push_back應該是相同的.
        emplace_pack是比較新的寫法,一般建議用emplace_back
        目前emplace_pack可以直接傳value_type constructor的引數
 */
void test_vector_emplace_1();
void test_vector_emplace_2();
void test_vector_emplace_3();
void test_vector_alg();


void test_deque_alg();

void test_array_alg();

void test_list_alg();

// 單向 list
void test_forward_list_alg();

void test_bitset_1();
void test_bitset_2();

void test_stack();
void test_queue();
void test_piority_queue();



// map
void test_map_1();
void test_map_2();
void test_map_3();

// multimap
void test_multi_map();

// unorder map
void test_unordered_map_1();
void test_unordered_map_2();

// 資料不能重複, 不能修改value, 可以新增移除資料
void test_set();


void test_container_func_1();
void test_container_func_2();
void test_container_func_3();


#endif
