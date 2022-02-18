#ifndef STATIC_LIB_2_H
#define STATIC_LIB_2_H

/*
這邊沒使用extern "C", 會有name mangling的問題,會需要再呼叫端加上extern "C".
可以參考 https://medium.com/@fcamel/%E5%BE%9E-c-%E5%91%BC%E5%8F%AB-c-%E5%87%BD%E5%BC%8F%E7%9A%84%E9%81%8E%E7%A8%8B%E7%90%86%E8%A7%A3%E7%A8%8B%E5%BC%8F%E7%B7%A8%E8%AD%AF-%E9%80%A3%E7%B5%90%E7%9A%84%E5%8E%9F%E7%90%86-4844f02ef004
*/

#ifdef __cplusplus
extern "C" {
#endif



int static_lib_func_2();



#ifdef __cplusplus
} // end extern "C"
#endif


#endif
