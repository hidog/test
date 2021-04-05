/* Lapack routines with a convenient Matrix wrapper. 
 */

#include <malloc.h>
#include <math.h>
#include "matrix.h"
#include <cstring>
#include <cstdlib>


/* The Fortran routines expect column-major matrices, but our Matrices are
 * row-major. These functions try to hide this difference as much as possible.
 */

/* Returns the determinant of A. */
Real MatrixDeterminant(Matrix A)
{
  int   *ipvt;
  int    info;
  Real   det = 1.0;
  int    c1, neg = 0;
  Matrix tmp;

  tmp = MatrixCopy(A);
  ipvt = Allocate(tmp->height, int);
  /* Note width and height are reversed */
  F77_FCN(dgetrf)(&tmp->width, &tmp->height, tmp->data[0],
                  &tmp->height, ipvt, &info);
  if(info > 0) {
    /* singular matrix */
    return 0.0;
  }

  /* Take the product of the diagonal elements */
  for (c1 = 0; c1 < tmp->height; c1++) {
    det *= tmp->data[c1][c1];
    if (ipvt[c1] != (c1+1)) neg = !neg;
  }
  free(ipvt);
  MatrixFree(tmp);

  /* Since tmp is an LU decomposition of a rowwise permutation of A,
     multiply by appropriate sign */
  return neg?-det:det;
}

/* Modifies B to have the solution x to A * x = B. 
 * Returns 0 if an error occurred.
 */
int MatrixSolve(Matrix A, Real *B)
{
  int *ipvt;
  int info;
  Matrix tmp;
  char job;
  int nrhs;

  tmp = MatrixCopy(A);
  ipvt = Allocate(tmp->height, int);
  /* Note width and height are reversed */
  F77_FCN(dgetrf)(&tmp->width, &tmp->height, tmp->data[0],
		  &tmp->height, ipvt, &info);
  if(info > 0) {
    fprintf(stderr, "MatrixSolve: dgetrf reports singular matrix\n");
    return 0;
  }
  else {
    job = 'T';
    nrhs = 1;
    F77_FCN(dgetrs)(&job, &tmp->width, &nrhs, tmp->data[0], &tmp->height,
		    ipvt, B, &tmp->height, &info);
  }
  free(ipvt);
  MatrixFree(tmp);
  return 1;
}

/* Modifies A to be the inverse of A: inv(A) * A = I. 
 * Returns 0 if an error occurred.
 */
int MatrixInvert(Matrix A)
{
  int *ipvt;
  int info, lwork;
  Real *work;

  ipvt = Allocate(A->height, int);
  /* Note width and height are reversed */
  F77_FCN(dgetrf)(&A->width, &A->height, A->data[0],
		  &A->height, ipvt, &info);
  if(info > 0) {
    fprintf(stderr, "MatrixInvert: dgetrf reports singular matrix\n");
    return 0;
  }
  else {
    lwork = A->height * 3;
    work = Allocate(lwork, Real);
    F77_FCN(dgetri)(&A->height, A->data[0], &A->width, ipvt,
		    work, &lwork, &info);
    free(work);
    if(info > 0) {
      fprintf(stderr, "MatrixInvert: dgetri reports singular matrix\n");
      return 0;
    }
  }
  free(ipvt);
  return 1;
}

/* Fills vr and vi with the real and imaginary parts of the eigenvalues of A.
 * If vr or vi is NULL, that part of the result will not be returned.
 * Returns 0 if an error occurred.
 */
int MatrixEigenvalues(Matrix A, Real *vr, Real *vi)
{
  char jobvl = 'N', jobvr = 'N';
  int rank = A->height;
  double *dup;
  int one = 1;
  int lwork;
  double *work;
  int info;
  double *wr, *wi;

  if(vr == NULL) wr = Allocate(rank, double);
  else wr = vr;
  if(vi == NULL) wi = Allocate(rank, double);
  else wi = vi;

  /* make a copy since dgeev clobbers A */
  dup = Allocate(A->height * A->width, double);
  memcpy(dup, A->data[0], A->height * A->width * sizeof(double));

  lwork = 3 * A->height;
  work = Allocate(lwork, double);

  F77_FCN (dgeev) (&jobvl, &jobvr, &rank, dup, &A->height,
		   wr, wi, NULL, &one, NULL, &one, work, &lwork, &info);
  free(dup);
  free(work);
  if(vr == NULL) free(wr);
  if(vi == NULL) free(wi);
  if(info > 0) {
    fprintf(stderr, "MatrixEigenvalues failed\n");
    return 0;
  }
  return 1;
}

/* Makes lower triangular R such that R * R' = A. 
 * Modifies A.
 * Returns 0 if an error occurred.
 */
int MatrixCholesky(Matrix A)
{
  char uplo = 'U';
  int rank = A->height;
  int info;
  int i,j;

  F77_FCN (dpotrf) (&uplo, &rank, A->data[0], &A->height, &info);
  if(info > 0) {
    fprintf(stderr, "MatrixCholesky: matrix is not positive definite\n");
    return 0;
  }
  /* clear out the upper triangular */
  for(i=0;i<A->height;i++)
    for(j=i+1;j<A->width;j++)
      A->data[i][j] = 0.0;
  return 1;
}
