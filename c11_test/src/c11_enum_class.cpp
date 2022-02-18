#include "c11_enum_class.h"
#include <iostream>

using namespace std;



enum class Color
{
    red,
    green,
    blue,
};



enum class Character : char 
{
    A = 'A',
    B = 'B',
    C = 'C',
};


// 必須是整數型別,不可以是浮點數
enum class Animal : unsigned int 
{
    dog = 0,
    cat = 4,
};



namespace c11 {


void enum_class_test()
{
    Color color = Color::red;

    cout << "sizeof Color = " << sizeof(Color) << endl 
         << "value of red = " << static_cast<unsigned int>(color) << endl;

    Character c = Character::B;
    cout << "sizeof Character = " << sizeof(Character) << endl
         << "value of B = " << static_cast<char>(c) << endl;

    Animal animal = Animal::dog;
    cout << "sizeof Animal = " << sizeof(Animal) << endl
         << "value of dog = " << static_cast<unsigned int>(animal) << endl;
}



} // end namespace c11