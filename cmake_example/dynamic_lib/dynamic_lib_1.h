#ifndef DYNAMIC_LIB_1_H
#define DYNAMIC_LIB_1_H

#include "def.h"


/* 
    windows���x���[ dll export, �|�y���sĶ���~.

    �X��macro�Ѧ�.
    https://docs.microsoft.com/zh-tw/cpp/preprocessor/predefined-macros?view=msvc-160
*/



class LIBRARY_API DynamicObj
{
public:
    DynamicObj();
    ~DynamicObj();

    void test_func();
private:
};



LIBRARY_API int dynamic_func_1();


/* 
    �n���explicit���J, �h�ݭn�[�Jextern "C"
    ���M�|�o�Ϳ��~.
*/
extern "C" {
LIBRARY_API int dynamic_func_4();
}




#endif
