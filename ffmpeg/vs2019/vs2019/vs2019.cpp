#include <iostream>
#include "../../src/demux.h"


int main()
{

    Demux   demux;
    demux.open_file( "D:\\code\\test.mkv", "D:\\code\\video.dat", "D:\\code\\audio.dat" );
    demux.init();    
    demux.open_output();
    demux.run();
    demux.flush();
    demux.close_output();
    demux.end();

    system("PAUSE");
}

