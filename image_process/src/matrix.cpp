#include "matrix.h"


#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

//#include <iostream>
//using namespace std;




int MatrixInvert(Matrix A)
{
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<double> AA;
	AA.resize( A->width, A->height );

	for( int i = 0; i < A->width; i++ )
		for( int j = 0; j < A->height; j++ )
			AA( i, j )	=	A->data[i][j];

	// create a permutation matrix for the LU-factorization
	pmatrix pm(AA.size1());

	// perform LU-factorization
	int res = lu_factorize(AA, pm);
	if (res != 0)
		return 0;

	matrix<double>	inverse(AA.size1(),AA.size2() );

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<double> (AA.size1()));

	// backsubstitute to get the inverse
	lu_substitute(AA, pm, inverse);


	for( int i = 0; i < A->width; i++ )
		for( int j = 0; j < A->height; j++ )
			A->data[i][j]	=	inverse( i, j );

	return 1;
}


int MatrixSolve(Matrix A, Real *B)
{
	typedef permutation_matrix<std::size_t> pmatrix;

	// create a working copy of the input
	matrix<double> AA;
	AA.resize( A->width, A->height );

	for( int i = 0; i < A->width; i++ )
		for( int j = 0; j < A->height; j++ )
			AA( i, j )	=	A->data[i][j];

	// create a permutation matrix for the LU-factorization
	pmatrix pm(AA.size1());

	// perform LU-factorization
	int res = lu_factorize(AA, pm);
	if (res != 0)
		return 0;

	matrix<double>	inverse(AA.size1(),AA.size2() );

	// create identity matrix of "inverse"
	inverse.assign(identity_matrix<double> (AA.size1()));

	// backsubstitute to get the inverse
	lu_substitute(AA, pm, inverse);

	vector<double>	b,	x;
	b.resize( AA.size1() );
	x.resize( AA.size1() );

	for( int i = 0; i < AA.size1(); i++ )
		b(i)	=	B[i];

	x	=	prod( inverse, b );

	for( int i = 0; i < AA.size1(); i++ )
		B[i]	=	x(i);


	return	1;
}