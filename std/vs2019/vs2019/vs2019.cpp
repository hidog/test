
#include <iostream>
#include "file.h"
#include "timer.h"

int main()
{
    std::cout << "file size example : " << get_file_size() << "\n";

    Timer timer;
    timer.start();

    for( int i = 0; i < 100000; i++ )
        std::cout << ".";
    std::cout << "\n";

    timer.end();

    int hour, min, sec, msec, totalms;
    timer.get_time( hour, min, sec, msec, totalms );
    std::cout << "timer = " << timer.get_time() << "ms\n";
    std::cout << hour << " " << min << " " << sec << " " << msec << " " << totalms  << "\n";


    system("PAUSE");
    return 0;
}

