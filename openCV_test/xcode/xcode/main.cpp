#include <iostream>

#include "opencv2/opencv.hpp"

#include "base_test.h"
#include "filter_test.h"
#include "base_test_2.h"
#include "improcess.h"




int main()
{
    // base
    load_image_1();
    load_image_2();
    create_image();
    image_convert();
    
    image_test_1a();
    image_test_1b();
    image_test_2();
    
    filter_test();
    
    point_test();
    liner_blend(); 
    fourier_transform();

    // filter
    sobel_test();
    opencv_tutorial();
    laplace_test();

    // base 2
    generate_test_image();
    blur_test();

    // image process
    my_canny::canny_test();
    my_canny::canny_test_2();
    histogram_test();
    my_harris::harris_corner_test();

    return 0;
}
