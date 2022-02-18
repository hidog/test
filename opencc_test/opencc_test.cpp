#include "opencc_test.h"

#include <opencc.h>
#include <boost/locale.hpp>
#include <stdio.h>
#include <fstream>


void opencc_test_2()
{
    // note: 原本想用這個方式做編碼轉換, 但轉換沒成功.

    // use boost convert utf8 <-> big5, gb2312
    std::ifstream   in{ "path" };
    std::ofstream   out{ "path" };

    char str[4096];
    std::string gb2312_str, big5_utf8_str, gb_utf8_str, big5_str;
    std::string gb2312{"gb2312"}, big5{"big5"};


    try {
        const opencc::SimpleConverter converter{OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD};

        while( in.getline( str, 4096 ) )
        {   
            std::cout << str << std::endl;

            gb2312_str = str;

            gb_utf8_str = boost::locale::conv::to_utf<char>( gb2312_str, gb2312 );

            big5_utf8_str = converter.Convert( gb_utf8_str );

            //std::string big5 = boost::locale::conv::between( str, "big5", "gb2312" );

            big5_str = boost::locale::conv::from_utf<char>( big5_utf8_str, big5 );

            std::cout << big5_str << std::endl;

            //boost::locale::info::;
            out << big5_str << "\n";
        }

        in.close();
        out.close();

    }
    catch( std::exception e ) 
    {
        printf("err = %s", e.what() );
    }

}






void opencc_test_1()
{
    /*
    這個套件的功能是將 utf8 裡面的 簡體/繁體 中文互轉
    如果需要轉成 big5 或 gb2312 需要搭配其他套件
    例如 iconv, boost::locale::conv, QTextCodec.

    如果設定簡中轉繁中,丟繁中進去,效果是沒有轉換.
    還沒找到如何判斷字串是繁中還是簡中的做法,可能要自己寫
    */

    try 
    {
        const opencc::SimpleConverter converter(OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD);

        FILE *p = fopen( "D:\\code\\test.txt", "r" );
        FILE *q = fopen( "D:\\code\\test2.txt", "w+" );

        if( p == NULL || q == NULL )
            throw std::exception("load file fail.");

        char str[1000];
        while( fscanf( p, "%s", str ) != EOF )
        {
            std::string big5 = converter.Convert(str);  
            //std::cout << big5 << std::endl;
            //std::cout << str << std::endl;
            fprintf( q, "%s\n", big5.c_str() );
        }

        fclose(p);
        fclose(q);

    } 
    catch( std::exception e ) 
    {
        printf("err = %s", e.what() );
    }

}
