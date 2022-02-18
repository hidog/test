#ifndef SCANF_REGULAR_H
#define SCANF_REGULAR_H

/*
https://medium.com/@nemokuo77/%E6%AD%A3%E8%A6%8F%E8%A1%A8%E7%A4%BA%E5%BC%8F-regular-expression-22d1492c1830
http://ccckmit.wikidot.com/regularexpression
https://edisonx.pixnet.net/blog/post/74516127
http://programmermagazine.github.io/201312/htm/article2.html
https://blog.csdn.net/lbird/article/details/1724429
https://www.cnblogs.com/clover-toeic/p/3727028.html





%[abf]               只接受 a, b, f, 三個字元.
%[^ABO]              斷點, 找到第一個非A,B,O的字元當開始 
%[a-g]               接受a~g的字元
%[A-Z]               接受A~Z的字元
%[1-37-9j-lM-P]      接受 1~3, 7~9, j~l, M~P
%[0-9,.!@#$%^&*() ]  抓0~9 跟後面的特殊字元. 空白字元也算在內
%[0-9!]              只容許0-9, !
%[^@]                斷點,遇到 @ 停止. 
%[^0-9]              斷點,遇到數字暫停.
%5[A-Z]              A~Z 抓五個字元
%*s                  跳過這個字串
%[^^]                抓到第一個非 ^ 的符號
%[\\^]               抓到第一個 ^ 的符號. (須加上斜線)
%[^1-9]              非1~9的字串

scanf_regular_test_2的過濾作法會在 scanf_regular_test_9 介紹另一個做法
*/


void scanf_regular_test_1();
void scanf_regular_test_2();
void scanf_regular_test_3();
void scanf_regular_test_4();
void scanf_regular_test_5();
void scanf_regular_test_6();
void scanf_regular_test_7();
void scanf_regular_test_8();
void scanf_regular_test_9();
void scanf_regular_test_10();
void scanf_regular_test_11();
void scanf_regular_test_12();
void scanf_regular_test_13();
void scanf_regular_test_14();





void cstyle_regular_expression_func();




#endif