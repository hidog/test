#include "scanf_regular.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>




void scanf_regular_test_1()
{
    char    input[100] = "ababooaaff";
    char    str[100];

    // %[abo]只接受 a, b, o 三個字元. (貪婪,取最長)
    // 輸出 ababooaa, 碰到 f, 暫停
    int res = sscanf( input, "%[abo]", str );
    if( res <= 0 )
        printf("error\n");

    printf( "str = %s\n", str );
}





void scanf_regular_test_2()
{
    char    input[100] = "ababooaaffAAOOBABABAuiyYUIIiyiyi uuiYYUIUY ";
    char    str1[100] = "", str2[100] = "", str3[100] = "", str4[100] = "", str5[100];

    /*
        我的目標是抓 [abo], [ABO], [YUIyui]
        %[^ABO] 會過濾掉非 ABO 的字元.  (抓出ff)
        沒過濾掉的話,會把後面的字串一起抓進來.
        scanf_regular_test_9 會介紹另一個過濾的做法
    */
    int res = sscanf( input, "%[abo]%[^ABO]%[AOB]%[YUIyui] %[YUIyui]", str1, str2, str3, str4, str5 );
    if( res <= 0 )
        printf("error\n");

    printf( "str1 = %s\nstr3 = %s\nstr4 = %s\nstr5 = %s\n", str1, str3, str4, str5 );
}





void scanf_regular_test_3()
{
    char    input[100] = "abcdefghijk ABCDEFGHIJKLMNOPQRST 1234 123789jklMNOPz";
    char    str1[100] = "", str2[100] = "", str3[100] = "", str4[100] = "";

    /*
        1-37-9  表示抓 1~3, 7~9的字元
        %[a-g]hijk        會過濾掉 hijk
        %[1-37-9j-lM-P]4  會過濾掉 4
        實際應用的時候應該會需要一個字串來捕捉不再目標的字元
    */
    int res = sscanf( input, "%[a-g]hijk %[A-Z] %[1-37-9j-lM-P]4 %[1-37-9j-lM-P]", str1, str2, str3, str4 );
    if( res <= 0 )
        printf("error\n");

    printf("str1 = %s\nstr2 = %s\nstr3 = %s\nstr4 = %s\n", str1, str2, str3, str4 );
}




void scanf_regular_test_4()
{
    char    input[100] = "12!^34,.@#$)%^ &*()876";
    char    str[100] = "";

    /*
        0-9abc  會抓 0~9, a, b, c字元.
        範例用特殊字元驗證這個語法可以通用於特殊字元
    */
    int res = sscanf( input, "%[0-9,.!@#$%^&*() ]", str );
    if( res <= 0 )
        printf("error\n");

    printf("str = %s\n", str );
}





void scanf_regular_test_5()
{
    char    input[100] = "test.abc@goooooo.com.tw";
    char    str1[100] = "", str2[100] = "";

    /*
        ^@ 抓到 @ 結束.
        用來分解 e-mail 格式
    */
    int res = sscanf( input, "%[^@]@%s", str1, str2 );
    if( res <= 0 )
        printf("error\n");

    printf("str1 = %s\nstr2 = %s\n", str1, str2 );
}





void scanf_regular_test_6()
{
    char input[300] = "a;fjla35780235aaj;oa309a23;0a35a0;abvjl;b1037j;lvmla;sfa;f23a;asdlf2307af;lv0094o1;navd087a3;alv0;83a;o3av;873aaa";
    char str1[100] = "", str2[100] = "";
    int  integer;
    size_t  shift = 0;

    /*
        將輸入字串裡面的數字挑出來轉成整數
    */
    while(true)
    {
        int res = sscanf( input + shift, "%[^0-9]%[0-9]", str1, str2 );

        if( res <= 0 )
            break;
        integer = atoi(str2);
        shift += strlen(str1) + strlen(str2);

        // 最後一次抓到 str1 = aaa 的時候, str2會是873. 目前只想的到這個方式做停止判斷
        if( shift >= strlen(input) )
            break;

        printf("get integer = %d\n", integer );
    }
}






void scanf_regular_test_7()
{
    char    input[100] = "ABCDEFGHIJKLMN";
    char    str[100] = "";

    /*
        A~Z 抓五個字元
    */
    int res = sscanf( input, "%5[A-Z]", str );
    if( res <= 0 )
        printf("error\n");

    printf("str = %s\n", str );
}






void scanf_regular_test_8()
{
    char    input[100] = "ABCDEFG HIJKLMN";
    char    str[100] = "";

    /*
        %*s  會跳過這個字串.
    */
    int res = sscanf( input, "%*s%s", str );
    if( res <= 0 )
        printf("error\n");

    printf("str = %s\n", str ); // 抓到 HIJKLMN
}





void scanf_regular_test_9()
{
    char    input[100] = "ababooaaffAAOOBABABAuiyYUIIiyiyiuuiYYUIUY";
    char    str1[100] = "", str2[100] = "", str3[100] = "";

    /*
        承 scanf_regular_test_2, 用比較好的過濾做法
        %*s 可以跳過字串.
        %*[^ABO]  跳過非 A,B,O 的字串. 不需要用多用一個buffer去存資料.  (這個範例會跳過 ff )
    */
    int res = sscanf( input, "%[abo]%*[^ABO]%[AOB]%[YUIyui]", str1, str2, str3 );
    if( res <= 0 )
        printf("error\n");

    printf( "str1 = %s\nstr2 = %s\nstr3 = %s\n", str1, str2, str3 );
}





void scanf_regular_test_10()
{
    char    input[100] = "http://www.google.com:1234?name=Andy?height=165?width=414";
    char    url[100] = "", str[100] = "";
    int     port = 0;

    /*
        切開網址, 並且印出參數
    */
    int res = sscanf( input, "%*[^:]%*[:/]%[^:]:%d%s", url, &port, str );
    if( res <= 0 )
        printf("error\n");

    printf( "url = %s\nport = %d\nstr = %s\n\n", url, port, str );

    // 切開參數
    size_t shift = 0;
    char key[100] = "", value[100] = "";
    while(true)
    {
        // 底下兩行執行結果相同.
        // res = sscanf( str + shift, "?%[^=]=%[^?]", key, value );
        res = sscanf( str + shift, "%*[?]%[^=]%*[=]%[^?]", key, value );


        if( res <= 0 )
            printf("error\n");            
        printf("key = %s, value = %s\n", key, value );
        
        shift += strlen(key) + strlen(value) + 2; // 2 = ?, =
        if( shift >= strlen(str) )
            break;
    }
}





void scanf_regular_test_12()
{
    char input[100] = "123456789!!!!!!!!!^";
    char str[100];

    int res = sscanf( input, "%[0-9!]", str );
    if( res <= 0 )
        printf("error\n");

    printf("str = %s\n", str);
}






void scanf_regular_test_13()
{
    char input[100] = "abc!@#$defg^12345";
    char str1[100] = "", str2[100] = "";

    int res = sscanf( input, "%[^^]%*[\\^]%s", str1, str2 );
    if( res <= 0 )
        printf("error\n");

    printf( "str1 = %s\nstr2 = %s\n", str1, str2 );
}




void scanf_regular_test_14()
{
    char input[100] = "abcde000004";
    char str[100] = "";

    // 接受非1~9的字元
    int res = sscanf( input, "%[^1-9]", str );
    if( res <= 0 )
        printf("error\n");

    printf("str = %s\n", str);
}




void cstyle_regular_expression_func()
{
    scanf_regular_test_1();
    printf("\n");

    scanf_regular_test_2();
    printf("\n");

    scanf_regular_test_3();
    printf("\n");
    
    scanf_regular_test_4();
    printf("\n");

    scanf_regular_test_5();
    printf("\n");

    scanf_regular_test_6();
    printf("\n");

    scanf_regular_test_7();
    printf("\n");

    scanf_regular_test_8();
    printf("\n");

    scanf_regular_test_9();
    printf("\n");

    scanf_regular_test_10();
    printf("\n");

    scanf_regular_test_11();
    printf("\n");

    scanf_regular_test_12();
    printf("\n");

    scanf_regular_test_13();
    printf("\n");

    scanf_regular_test_14();
    printf("\n");
}





void scanf_regular_test_11()
{
    char input[100] = "2006:03:18-2006:04:18";
    char time1[100] = "", time2[100] = "";

    int res = sscanf( input, "%[0-9:]-%[0-9:]", time1, time2 );

    if( res <= 0 )
        printf("error\n");

    printf("time1 = %s, time2 = %s", time1, time2 );
}




