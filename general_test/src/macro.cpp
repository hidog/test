#include "macro.h"
#include <iostream>

using namespace std;


static int hight;
static double depth;


/*
    ## 的作用是把兩個字連接再一起
    可以用這個方式自動產生 function.
*/
#define GENERATE_GET_FUNC( NAME, TYPE ) \
    TYPE get_##NAME##_##TYPE() \
    { \
        return NAME; \
    }

#define GENERATE_SET_FUNC( NAME, TYPE ) \
    void set_##NAME##_##TYPE( TYPE p ) \
    { \
        NAME = p; \
    }


GENERATE_GET_FUNC( hight, int )
GENERATE_SET_FUNC( hight, int )

GENERATE_GET_FUNC( depth, double )
GENERATE_SET_FUNC( depth, double )


/*
    # 的作用是產生字串
*/
#define GENERATE_STR( STR ) #STR

/*
    #@ 產生字元
    只有windows平台支援的樣子
*/
#ifdef WIN32
#define GENERATE_CHARACTER( C ) #@C
#endif


#define LOG(...) \
    { \
        printf("[%s %s %d] %s\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__ ); \
    }



void macro_test_1()
{
    set_hight_int( 452 );
    int h = get_hight_int();

    set_depth_double( 44.12234 );
    double d = get_depth_double();

    cout << "h = " << h << ", d = " << d << endl;

    std::string str = GENERATE_STR(abcdefg);
    cout << "str = " << str << endl;

#ifdef WIN32
    char c = GENERATE_CHARACTER(z);
    cout << "c = " << c << endl;
#endif
}



void macro_test_2()
{
    LOG("this is macro test.");
    LOG("how to use this macro? We could add level to the macro.");
}




void test_macro_func()
{
    macro_test_1();
    macro_test_2();
}

