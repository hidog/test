#include "regular_express.h"

#include <regex>
#include <string>
#include <iostream>

using namespace std;





/*
    這個範例只能對英文單字做比對. 無法對句子做比對.
*/
void test_regex_1()
{
    //std::regex      reg("[A-Z]");  // 只會比對一個字元
    std::regex      reg("[A-Z]*");   // 會比對到整個字.  *表示0或是多個. 因此沒匹對到也會印出一個空字串.

    std::string     input[3] = { "this", 
                                 "This",
                                 "THIS" };
    std::smatch     match;
    size_t          i;

    for( i = 0; i < 3; i++ )
    {
        std::cout << "input " << i << std::endl;        
        bool res = std::regex_search( input[i], match, reg );   // 可以只印出一個字母.
        //bool res = std::regex_match( input[i], match, reg );    // 整個字符合才印出來
        if( res == false )
            std::cout << "not found.\n"; 
        else
            std::cout << "match str = " << match.str() << std::endl;
        
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


void test_regex_2()
{
    std::string target("baaaby");
    std::smatch sm;

    std::regex  re1("[a-z]*");
    std::regex_search( target, sm, re1 );
    std::cout << "entire match: " << sm.str(0) << '\n'
              << "submatch #1: " << sm.str(1) << '\n';

    std::regex  re2("a(a*)b");
    std::regex_search(target, sm, re2);
    std::cout << "entire match: " << sm.str(0) << '\n'
              << "submatch #1: "  << sm.str(1) << '\n';
    std::cout << std::endl;
}







void test_regex_3()
{
    //std::regex      reg("[A-Z]");  // 只會比對一個字元
    std::regex      reg("[A-Z]*");   // 會比對到整個字.  *表示0或是多個. 因此沒匹對到也會印出一個空字串.
    //std::regex      reg("[A-Z]+");   // 這個才會得到預期的結果,避免空白等字元都被比對進去

    std::string     input[3] = { "this is a dog. that is a cat", 
                                 "This is a dog. That is a cat.",
                                 "THIS IS A DOG. THAT IS A CAT." };
    size_t          i;

    for( i = 0; i < 3; i++ )
    {
        std::cout << "input " << i << std::endl;        
        auto begin =    std::sregex_iterator( input[i].begin(), input[i].end(), reg );  
        auto end   =    std::sregex_iterator();

        for( std::sregex_iterator itr = begin; itr != end; ++itr ) 
        {
            std::smatch match = *itr;
#if 1
            std::string match_str = match.str();
            if( match_str.size() > 0 ) // 過濾掉空字元. 如果使用 * 來比對, 會遇到這個問題
                std::cout << match_str << ", ";            
#else
            std:: cout << match.str() << ", ";
#endif
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}





void test_regex_4()
{
    std::regex      reg("[A-Z]{1,}[a-z]{0,}");   // 至少要有一個大寫字母的才會被比對出來

    std::string     input[3] = { "this is a dog. that is a cat", 
                                 "This is a dog. That is a cat.",
                                 "THIS IS A DOG. THAT IS A CAT." };
    std::smatch     match;
    size_t          i;

    for( i = 0; i < 3; i++ )
    {
        std::cout << "input " << i << std::endl;        
        auto begin =    std::sregex_iterator( input[i].begin(), input[i].end(), reg );  
        auto end   =    std::sregex_iterator();

        for( std::sregex_iterator itr = begin; itr != end; ++itr ) 
        {
            std::smatch match = *itr;
            if( match.str().size() > 0 )
                std:: cout << match.str() << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}






void test_regex_5()
{
    regex   reg{ R"([A-Za-z.]+[@]+[A-Za-z.]+[^;])" };
    string  input = "Andy.kao@google.com;gray@yahoo.com.tw";

    auto begin = sregex_iterator( input.begin(), input.end(), reg );
    auto end   = sregex_iterator();

    for( auto itr = begin; itr != end; ++itr )
    {
        smatch match = *itr;
        cout << match.str() << endl;
    }
    cout << endl;
}










void regular_express_func()
{
    test_regex_1();
    test_regex_2();
    test_regex_3();
    test_regex_4();
    test_regex_5();
}
