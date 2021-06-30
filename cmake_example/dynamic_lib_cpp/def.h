#ifndef DEF_H
#define DEF_H


#ifdef WIN32
#define DLL_EXPORT	__declspec(dllexport)
#else
#define DLL_EXPORT
#endif



/*
HIDOG_DLL_EXPORT	void	hidog_dll_test();


void	hidog_dll_test2();


class	HIDOG_DLL_EXPORT	hidog_abc_class_test
{
public:
    hidog_abc_class_test();
    //{
    //	std::cout << "hidog_abc_class_test" ;
    //}
};



https://docs.microsoft.com/zh-tw/cpp/build/importing-function-calls-using-declspec-dllimport?view=msvc-160
__declspec(dllimport)
加上import的範例

https://www.coder.work/article/12702  可以參考這個

*/


#endif