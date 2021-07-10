#include "enum_class.h"
#include <iostream>




enum class Color
{
    red,
    green,
    blue,
};



void c11_enum_class_test()
{
    Color color = Color::red;

    std::cout << static_cast<int>(color);
}
