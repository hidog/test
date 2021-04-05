#include <stdio.h>


#define M_PI 3.1415926


#ifndef Allocate
#define Allocate(n, t) (t*)malloc((n)*sizeof(t))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef sqr
#define sqr(a) ((a)*(a))
#endif

typedef double Real;
typedef struct MatrixStruct {
  Real **data;
  int width, height;
} *Matrix;

#ifdef Complex
/* X.h defines Complex to be zero. Very bad! */
#undef Complex
#endif
typedef struct { Real real, imag; } Complex;

typedef unsigned char uchar;
typedef Real MathFunc(Real x);





int MatrixInvert(Matrix A);
int MatrixSolve(Matrix A, Real *B);








///* basic.c *******************************************************************/
Real dot_product(Real *a, Real *b, int len);
//Matrix MatrixFilter(Matrix matrix, Matrix filter);
void MatrixSubtractRow(Matrix matrix, Real *row);
//Matrix MatrixColSum(Matrix matrix);
Matrix MatrixColMean(Matrix matrix);
//void MatrixMinMax(Matrix matrix, Real *minp, Real *maxp);
//void MatrixRescale(Matrix matrix, Real new_min, Real new_max);
//void MatrixTruncate(Matrix matrix, Real new_min, Real new_max);
//void MatrixTruncRescale(Matrix matrix, Real trunc_min, Real trunc_max, 
//			Real new_min, Real new_max);
//void MatrixApplyFunc(Matrix matrix, MathFunc *func);
//Matrix MatrixSub(Matrix matrix, int y1, int x1, int y2, int x2);
//Real MatrixSum(Matrix matrix);
Real MatrixMean(Matrix matrix);
void MatrixAddValue(Matrix matrix, Real value);
//void MatrixFill(Matrix matrix, Real value);
//void MatrixScale(Matrix matrix, Real factor);
//
void MatrixReadImage(Matrix matrix, FILE *fp);
//void MatrixWriteImage(Matrix matrix, FILE *fp);
void MatrixWriteVector(Matrix matrix, FILE *fp);
//void MatrixWrite(Matrix matrix, FILE *fp);
//
//Matrix MatrixFromArray(int height, int width, double *data);
Matrix MatrixCreate(int height, int width);
void MatrixFree(Matrix matrix);
//Matrix MatrixCopy(Matrix matrix);
//
///* math.c ********************************************************************/
//void MatrixMagnitude(Matrix A, Matrix B);
//
///* lapack.c ******************************************************************/
//double MatrixDeterminant(Matrix A);
//int MatrixSolve(Matrix A, Real *B);
//int MatrixInvert(Matrix A);
//int MatrixEigenvalues(Matrix A, Real *vr, Real *vi);
//int MatrixCholesky(Matrix A);
//
///* fft.c *********************************************************************/
//void MatrixFourier(Matrix A, Matrix *B);
//void MatrixTwoDimFourier(Matrix A, Matrix *B);
//
///* Fortran routines in LAPACK ************************************************/
//#define F77_FCN(f) f##_
//int F77_FCN(dgetrf)(const int*, const int*, double*, int*, int*, int*);
//int F77_FCN(dgetrs)(const char*, const int*, const int*, double*, 
//		    const int*, int*, double*, const int*, int*);
//int F77_FCN(dgetri)(const int*, double*, const int*, int*, double*, 
//		    const int*, int*);
//int F77_FCN(dgeev)(const char *jobvl, const char *jobvr, const int *n, 
//		   double *A, const int *lda, double *wr, double *wi, 
//		   double *vl, const int *ldvl, double *vr, const int *ldvr, 
//		   double *work, const int *lwork, int *info);
//int F77_FCN(dpotrf)(const char *uplo, const int *n, double *A,
//		    const int *lda, int *info);
//
///* Fortran routines in FFTPACK ***********************************************/
//int F77_FCN(cffti)(const int *n, Complex *wsave);
//int F77_FCN(cfftf)(const int *n, Complex *c, Complex *wsave);
//
///* Fortran routines in BIHAR *************************************************/
//int F77_FCN(dcffti)(const int *n, Complex *wsave);
//int F77_FCN(dcfftf)(const int *n, Complex *c, Complex *wsave);
//
///* pdf.c *********************************************************************/
//Matrix MatrixReadPDF(FILE *h);
//int MatrixWritePDF(FILE *h, Matrix m, char *dest_t);
