#include "opencc_test.h"

#include <opencc.h>
#include <boost/locale.hpp>
#include <stdio.h>
#include <fstream>


void opencc_test_2()
{
    // note: �쥻�Q�γo�Ӥ覡���s�X�ഫ, ���ഫ�S���\.

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
    �o�ӮM�󪺥\��O�N utf8 �̭��� ²��/�c�� ���夬��
    �p�G�ݭn�ন big5 �� gb2312 �ݭn�f�t��L�M��
    �Ҧp iconv, boost::locale::conv, QTextCodec.

    �p�G�]�w²�����c��,���c���i�h,�ĪG�O�S���ഫ.
    �٨S���p��P�_�r��O�c���٬O²�������k,�i��n�ۤv�g
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
