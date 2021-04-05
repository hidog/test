/* Matrix implementation. Simple operations.
 * A matrix is a structure of {height, width, data}. 
 * The data is double precision numbers, addressable both as a
 * two-dimensional array (m->data[row][col]) or a row-packed vector 
 * (m->data[0][index]).
 */

#include <malloc.h>
#include "matrix.h"
#include <cstring>
#include <cstdlib>

/* Prototypes ****************************************************************/
double dot_product(double *a, double *b, int len);
Matrix MatrixFilter(Matrix matrix, Matrix filter);
void MatrixSubtractRow(Matrix matrix, Real *row);
Matrix MatrixColSum(Matrix matrix);
Matrix MatrixColMean(Matrix matrix);
void MatrixMinMax(Matrix matrix, Real *minp, Real *maxp);
void MatrixRescale(Matrix matrix, Real new_min, Real new_max);
void MatrixTruncate(Matrix matrix, Real new_min, Real new_max);
void MatrixTruncRescale(Matrix matrix, Real trunc_min, Real trunc_max, 
			Real new_min, Real new_max);
void MatrixApplyFunc(Matrix matrix, MathFunc *func);
Matrix MatrixSub(Matrix matrix, int y1, int x1, int y2, int x2);
Real MatrixSum(Matrix matrix);
Real MatrixMean(Matrix matrix);
void MatrixAddValue(Matrix matrix, Real value);
void MatrixFill(Matrix matrix, Real value);
void MatrixScale(Matrix matrix, Real factor);

void MatrixReadImage(Matrix matrix, FILE *fp);
void MatrixWriteImage(Matrix matrix, FILE *fp);
void MatrixWriteVector(Matrix matrix, FILE *fp);
void MatrixWrite(Matrix matrix, FILE *fp);

Matrix MatrixFromArray(int height, int width, double *data);
Matrix MatrixCreate(int height, int width);
void MatrixFree(Matrix matrix);
Matrix MatrixCopy(Matrix matrix);

/* Functions *****************************************************************/

/* Vector dot product. */
double dot_product(double *a, double *b, int len)
{
  double r = 0.0;
  while(len--) r += *a++ * *b++;
  return r;
}

/* Apply a filter to a matrix. 
 * The middle value of the filter is at 
 *   ((filter->height+1)/2, (filter->width+1)/2).
 * Not very efficient; could have implemented some special cases (3x3).
 */
Matrix MatrixFilter(Matrix matrix, Matrix filter)
{
  int i,j,k,l;
  int halfy = filter->height/2, halfx = filter->width/2;
  Matrix out = MatrixCreate(matrix->height, matrix->width);
  for(i=0;i<matrix->height;i++) {
    for(j=0;j<matrix->width;j++) {
      Real sum = 0.0;
      for(k=0;k<filter->height;k++) {
	for(l=0;l<filter->width;l++) {
	  int ii = (i+k-halfy+matrix->height)%matrix->height,
	      ij = (j+l-halfx+matrix->width)%matrix->width;
	  sum += filter->data[k][l] * matrix->data[ii][ij];
	}
      }
      out->data[i][j] = sum;
    }
  }
  return out;
}

/* Vector difference */
static void subtract(Real *a, Real *b, int len)
{
  while(len--) *a++ -= *b++;
}

/* Subtract a vector from each row of a matrix */
void MatrixSubtractRow(Matrix matrix, Real *row)
{
  int i;

  for(i=0;i<matrix->height;i++) {
    subtract(matrix->data[i], row, matrix->width);
  }
}

/* Compute a row vector of column sums */
Matrix MatrixColSum(Matrix matrix)
{
  Matrix row;
  int i,j;

  row = MatrixCreate(1, matrix->width);
  MatrixFill(row, 0.0);
  for(i=0;i<matrix->height;i++) {
    for(j=0;j<matrix->width;j++) {
      row->data[0][j] += matrix->data[i][j];
    }
  }
  return row;
}

/* Compute a row vector of column means */
Matrix MatrixColMean(Matrix matrix)
{
  Matrix row;

  row = MatrixColSum(matrix);
  MatrixScale(row, (Real)1/matrix->height);
  return row;
}

/* Compute the maximum and minimum values in a matrix */
void MatrixMinMax(Matrix matrix, Real *minp, Real *maxp)
{
  int y,x;

  *minp = *maxp = matrix->data[0][0];
  for(y=0;y < matrix->height;y++) {
    for(x=0;x < matrix->width;x++) {
      if(matrix->data[y][x] < *minp) *minp = matrix->data[y][x];
      else if(matrix->data[y][x] > *maxp) *maxp = matrix->data[y][x];
    }
  }
}

/* Rescale the values in a matrix to range from new_min to new_max */
void MatrixRescale(Matrix matrix, Real new_min, Real new_max)
{
  Real old_min, old_max;

  MatrixMinMax(matrix, &old_min, &old_max);
  MatrixAddValue(matrix, -old_min);
  if(old_max != old_min)
    MatrixScale(matrix, (new_max - new_min) / (old_max - old_min));
  MatrixAddValue(matrix, new_min);
}

/* Truncate the values in a matrix, 
 * i.e. set anything below new_min to new_min, and same for new_max.
 */
void MatrixTruncate(Matrix matrix, Real new_min, Real new_max)
{
  int y,x;

  for(y=0;y<matrix->height;y++) {
    for(x=0;x<matrix->width;x++) {
      if(matrix->data[y][x] < new_min) matrix->data[y][x] = new_min;
      else if(matrix->data[y][x] > new_max) matrix->data[y][x] = new_max;
    }
  }  
}

/* Truncate the values in a matrix to [trunc_min, trunc_max] and then
 * rescale to range from new_min to new_max.
 */
void MatrixTruncRescale(Matrix matrix, Real trunc_min, Real trunc_max, 
			Real new_min, Real new_max)
{
  int i;
  double factor;

  factor = (new_max - new_min) / (trunc_max - trunc_min);
  for(i=0;i<matrix->height*matrix->width;i++) {
    if(matrix->data[0][i] <= trunc_min) matrix->data[0][i] = new_min;
    else if(matrix->data[0][i] >= trunc_max) matrix->data[0][i] = new_max;
    else 
      matrix->data[0][i] = (matrix->data[0][i] - trunc_min) * factor + new_min;
  }
}

/* Apply a unary math function to every element */
void MatrixApplyFunc(Matrix matrix, MathFunc *func)
{
  int y,x;

  for(y=0;y<matrix->height;y++) {
    for(x=0;x<matrix->width;x++) {
      matrix->data[y][x] = func(matrix->data[y][x]);
    }
  }
}

/* Extract the submatrix from (y1, x1) to (y2, x2), inclusive */
Matrix MatrixSub(Matrix matrix, int y1, int x1, int y2, int x2)
{
  Matrix result;
  int range_x, range_y, y, x;

  if((x1 < 0) || (y1 < 0) || (y2 < y1) || (x2 < x1) ||
     (x2 >= matrix->width) || (y2 >= matrix->height)) {
    fprintf(stderr,
	    "MatrixSub: bad region (%d, %d)-(%d, %d) on %d by %d matrix\n",
	    y1, x1, y2, x2, matrix->height, matrix->width);
  }

  range_x = x2 - x1 + 1;
  range_y = y2 - y1 + 1;
  result = MatrixCreate(range_y, range_x);
  for(y=0;y < range_y;y++) {
    for(x=0;x < range_x;x++) {
      result->data[y][x] = matrix->data[y+y1][x+x1];
    }
  }
  return result;
}

/* Sum of all elements */
Real MatrixSum(Matrix matrix)
{
  Real sum;
  int y,x;

  sum = 0;
  for(y=0;y < matrix->height;y++) {
    for(x=0;x < matrix->width;x++) {
      sum += matrix->data[y][x];
    }
  }
  return sum;
}

/* Mean of all elements */
Real MatrixMean(Matrix matrix)
{
  return MatrixSum(matrix) / matrix->width / matrix->height;
}

/* Add a constant value to all elements */
void MatrixAddValue(Matrix matrix, Real value)
{
  int y,x;

  for(y=0;y<matrix->height;y++) {
    for(x=0;x<matrix->width;x++) {
      matrix->data[y][x] += value;
    }
  }
}

/* Copy a matrix */
Matrix MatrixCopy(Matrix matrix)
{
  Matrix result;

  result = MatrixCreate(matrix->height, matrix->width);
  memcpy(result->data[0], matrix->data[0], 
	 matrix->width*matrix->height*sizeof(Real));
  return result;
}

/* Set all elements to a value */
void MatrixFill(Matrix matrix, Real value)
{
  int y,x;

  for(y=0;y<matrix->height;y++) {
    for(x=0;x<matrix->width;x++) {
      matrix->data[y][x] = value;
    }
  }
}

/* Multiply all elements by a factor */
void MatrixScale(Matrix matrix, Real factor)
{
  int y,x;

  for(y=0;y<matrix->height;y++) {
    for(x=0;x<matrix->width;x++) {
      matrix->data[y][x] *= factor;
    }
  }
}

/* Read a character file as a matrix */
void MatrixReadImage(Matrix matrix, FILE *fp)
{
  unsigned char *buffer;
  int i,bufsize;

  bufsize = matrix->width * matrix->height;
  buffer = Allocate(bufsize, unsigned char);
  if(!buffer) {
    fprintf(stderr, "Cannot allocate %d byte input buffer\n", 
	    bufsize*sizeof(unsigned char));
    exit(1);
  }
  fread(buffer, sizeof(unsigned char), bufsize, fp);

  for(i=0;i<bufsize;i++) {
    matrix->data[0][i] = (Real)buffer[i];
  }
  free(buffer);
}

/* Write a matrix as a character file by clipping */
void MatrixWriteImage(Matrix matrix, FILE *fp)
{
  unsigned char *buffer;
  int i,bufsize;
  Real v;

  bufsize = matrix->width * matrix->height;
  buffer = Allocate(bufsize, unsigned char);
  if(!buffer) {
    fprintf(stderr, "Cannot allocate %d byte output buffer\n", 
	    bufsize*sizeof(unsigned char));
    exit(1);
  }
  for(i=0;i<bufsize;i++) {
    v = matrix->data[0][i];
    if(v > 255) v = 255;
    else if(v < 0) v = 0;
    buffer[i] = (unsigned char)v;
  }

  fwrite(buffer, sizeof(unsigned char), bufsize, fp);
  free(buffer);
}

/* Write a matrix as a vector of floating-point numbers, one per line. */
void MatrixWriteVector(Matrix matrix, FILE *fp)
{
  int i;

  for(i=0;i<matrix->height*matrix->width;i++) {
    fprintf(fp, "%lg\n", matrix->data[0][i]);
  }
}

/* Write a matrix as a rectangle of numbers, one row per line */
void MatrixWrite(Matrix matrix, FILE *fp)
{
  int i,j;
  for(i=0;i<matrix->height;i++) {
    for(j=0;j<matrix->width;j++) {
      fprintf(fp, "%g ", matrix->data[i][j]);
    }
    fprintf(fp, "\n");
  }
}

/* Creates a Matrix whose data is given. The array becomes property
 * of the matrix; it will be freed when the matrix is. The array must
 * be height*width elements long.
 */
Matrix MatrixFromArray(int height, int width, double *data)
{
  Matrix result;
  int i;

  result = Allocate(1, struct MatrixStruct);
  result->width = width;
  result->height = height;
  result->data = Allocate(height, Real*);
  result->data[0] = data;
  for(i=1;i<height;i++) {
    result->data[i] = &result->data[0][i*width];
  }
  return result;
}

/* Create a new, uninitialized matrix */
Matrix MatrixCreate(int height, int width)
{
  double *data = Allocate(width*height, Real);
  if(!data) {
    fprintf(stderr, "Cannot allocate %d by %d matrix\n", height, width);
    return NULL;
  }
  return MatrixFromArray(height, width, data);
}

/* Destroy a matrix */
void MatrixFree(Matrix matrix)
{
  free(matrix->data[0]);
  free(matrix->data);
  free(matrix);
}

