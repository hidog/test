#ifndef _ZERNIKE_TOOL_H
	#define _ZERNIKE_TOOL_H

#include <complex>
using namespace std;


#define		N_MAX	12




// 產生階層的表
void	factorial_list();

// 讀取階層的表
void	factorial_load( int *a );

// 產生 r 的表
void	r_list( int *fac );

// 讀取 r 的資料
void	r_load( int *r_data );

// 回傳 R_nm(x,y) 的值
double	R_nm( int n, int m, double x, double y, int *r_data );

// 回傳 V_nm(x,y) 的值
void	V_nm( int n, int m, double x, double y, int *r_data, complex<double> &v );



#endif