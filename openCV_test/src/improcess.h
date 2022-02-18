#ifndef IMPROCESS_H
#define IMPROCESS_H


namespace my_canny {

void canny_test();
int canny_test_2();

} // end namespace my_canny


void histogram_test();



namespace my_harris {

void harris_corner_test();
void harris_func( int, void* );

}; // end namespace my_harris




#endif