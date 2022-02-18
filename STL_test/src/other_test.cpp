#include "other_test.h"

#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>



using namespace std;




void test_char_string()
{
    std::string     str =   "This is a dog.";

    int count1 = (int)count_if( str.begin(), str.end(), [](char c) {
            return isupper(c);
        } );

#if defined(UNIX) || defined(MACOS)
    size_t count2 = (int)count_if( str.begin(), str.end(), [](char c) {
            return isspace(c);
        } );
#else
    size_t count2 = count_if( str.begin(), str.end(), std::isspace );
#endif

    cout << "count1 = " << count1 << endl;
    cout << "count2 = " << count2 << endl;

    // stoi, to_string, etc.
    std::string height_str = "12343";
    int height = stoi(height_str);

    int width = 5151;
    std::string width_str = to_string(width);

    cout << "print: " << height_str << " " << height << " " << width_str << " " << width << endl;
    

}



