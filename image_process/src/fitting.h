#ifndef _FITTING_H
#define _FITTING_H


#include "ceres/ceres.h"
#include "gflags/gflags.h"

//#include "../timer.h"

#include <fstream>


// 最大迭代數
#define MAX_ITERATOR 1000



// ellipse fit
double		ellipse_fit( double x, double y, double w, double l );

// circle fit
double		circle_fit( double cx, double cy, double w, double l ) ;

#endif