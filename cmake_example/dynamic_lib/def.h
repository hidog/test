#ifndef DEF_H
#define DEF_H


#ifdef WIN32
#   ifdef DYNAMIC_LIB
#       define LIBRARY_API __declspec(dllexport)
#   else
#       define LIBRARY_API __declspec(dllimport)
#   endif
#else
#   define LIBRARY_API
#endif



/*

https://docs.microsoft.com/zh-tw/cpp/build/importing-function-calls-using-declspec-dllimport?view=msvc-160
__declspec(dllimport)
�[�Wimport���d��

https://www.coder.work/article/12702  �i�H�Ѧҳo��

__cdecl  ���Ŭݤ@�U�o��

*/


#endif
