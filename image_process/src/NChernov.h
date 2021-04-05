#ifndef _NCHERNOV_H
#define _NCHERNOV_H

#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <fstream>
#include <cstdlib>

using namespace std;


namespace	NChernov
{






//  define precision by commenting out one of the two lines:
//typedef double reals;       //  defines reals as double (standard for scientific calculations)
typedef long double reals;  //  defines reals as long double 
//   Note: long double is an 80-bit format (more accurate, but more memory demanding and slower)
typedef long long integers;
//   next define some frequently used constants:
const reals One=1.0L,Two=2.0L,Three=3.0L,Four=4.0L,Five=5.0L,Six=6.0L;
const reals Pi=3.141592653589793238462643383L;
const reals REAL_MAX=numeric_limits<reals>::max();
const reals REAL_MIN=numeric_limits<reals>::min();
const reals REAL_EPSILON=numeric_limits<reals>::epsilon();







/************************************************************************
			DECLARATION OF THE CLASS DATA
************************************************************************/
// Class for Data
// A data has 5 fields: 
//       n (of type int), the number of data points 
//       X and Y (arrays of type reals), arrays of x- and y-coordinates
//       meanX and meanY (of type reals), coordinates of the centroid (x and y sample means)
class Data
{
public:

	int n;
	reals *X;		//space is allocated in the constructors
	reals *Y;		//space is allocated in the constructors
	reals meanX, meanY;

	// constructors
	Data();
	Data(int N);
	Data(int N, reals X[], reals Y[]);

	// routines
	void means(void);
	void center(void);
	void scale(void);
	void print(void);

	// destructors
	~Data();
};



/************************************************************************
			DECLARATION OF THE CLASS CIRCLE
************************************************************************/
// Class for Circle
// A circle has 7 fields: 
//     a, b, r (of type reals), the circle parameters
//     s (of type reals), the estimate of sigma (standard deviation)
//     g (of type reals), the norm of the gradient of the objective function
//     i and j (of type int), the iteration counters (outer and inner, respectively)
class Circle
{
public:

	// The fields of a Circle
	reals a, b, r, s, g;
	int i, j;

	// constructors
	Circle();
	Circle(reals aa, reals bb, reals rr);

	// routines
	void print(void);

	// no destructor we didn't allocate memory by hand.
};







//~~~~~~~~~~~~~~~~~~~~~~~~~~functions~~~~~~~~~~~~~~~~~~~~~~~
reals	pythag(reals a, reals b);
void	RandomNormalPair( reals& x, reals& y );
void	SimulateArc(Data& data, reals a, reals b, reals R, reals theta1, reals theta2, reals sigma);
void	SimulateRandom(Data& data, reals Window);
reals	Sigma (Data& data, Circle& circle);
reals	SigmaReduced (Data& data, Circle& circle);
reals	OptimalRadius (Data& data, Circle& circle);
bool	isfinite(reals a);


// fit by CL
Circle	CircleFitByCL (Data& data, Circle& Ini, reals LambdaIni);
Circle	Perturb (Circle& New, Circle& Old, reals range);

// fit by hyper
void	CircleFitByHyper (Data& data, Circle& circle);

// fit by kasa
void	CircleFitByKasa (Data& data, Circle& circle);

// fit by LMFull
void	CircleFitByLMFull (Data& data, Circle& circleIni, reals LambdaIni, Circle& circle);

// fit by LMReduced
void	CircleFitByLMReduced (Data& data, Circle& circleIni, reals LambdaIni, Circle& circle);

// fit by pratt
void	CircleFitByPratt (Data& data, Circle& circle);

// fit by taubin
void	CircleFitByTaubin (Data& data, Circle& circle);

};




#endif