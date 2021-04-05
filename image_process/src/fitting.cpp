/*===============================================================================================

	呼叫ceres solver,用來處理 variogram fit的功能

/*===============================================================================================*/

#include "stdafx.h"


#include "fitting.h"


using ceres::AutoDiffCostFunction;
using ceres::CostFunction;
using ceres::Problem;
using ceres::Solver;
using ceres::Solve;

using namespace std;


/*===============================================================================================
	定義 Gaussian
/*===============================================================================================*/
class FitEllipse 
{
	public:
		FitEllipse( double x, double y ) : x_(x), y_(y) {}

		/*---------------------------------------------------------
			y = Ax^2 + Bxy + Cy^2 + Dx + Ey + F
		/*---------------------------------------------------------*/
		template <typename T> bool operator()(const T* const A, const T* const B, const T* const C, const T* const D, const T* const E, const T* const F,   T* residual) const 
		//template <typename T> bool operator()( const T* const B, const T* const C,  const T* const D, const T* const E, const T* const F,   T* residual) const 
		//template <typename T> bool operator()( const T* const A, const T* const B,  const T* const D, const T* const E, const T* const F,   T* residual) const 
		{
			//residual[0]		=	1.0*T(x_)*T(x_) + B[0]*T(x_)*T(y_) + C[0]*T(y_)*T(y_) + D[0]*T(x_) + E[0]*T(y_) + F[0];		// 為了normalized首項係數 = 1
			//residual[0]		=	A[0]*T(x_)*T(x_) + B[0]*T(x_)*T(y_) + 1.0*T(y_)*T(y_) + D[0]*T(x_) + E[0]*T(y_) + F[0];		
			residual[0]		=	A[0]*T(x_)*T(x_) + B[0]*T(x_)*T(y_) + C[0]*T(y_)*T(y_) + D[0]*T(x_) + E[0]*T(y_) + F[0];
			//residual[0]		=	A[0]*T(x_)*T(x_) + B[0]*T(x_)*T(y_) + C[0]*T(y_)*T(y_) + D[0]*T(x_) + E[0]*T(y_) + T(30.0);
			return true;
		}

	private:
		const double	x_;
		const double	y_;
};


/*===============================================================================================
	定義 圓 
/*===============================================================================================*/
class FitCircle
{
	public:
		FitCircle( double x, double y ) : x_(x), y_(y) {}

		/*---------------------------------------------------------
			y = (x-h)^2 + (y-k)^2 - r^2
		/*---------------------------------------------------------*/
		template <typename T> bool operator()(const T* const h, const T* const k, const T* const r,  T* residual) const  
		{
			residual[0]		=	(T(x_) - h[0])*(T(x_) - h[0]) + (T(y_) - k[0])*(T(y_) - k[0]) - r[0]*r[0];
			return true;
		}

	private:
		const double	x_;
		const double	y_;
};





/*===============================================================================================
	處理Gaussian的實做
/*===============================================================================================*/
double		ellipse_fit( double cx, double cy, double w, double l ) 
{

	int			i;
	int			count;
	double		A,	B,	C,	D,	E,	F;
	double		x[3000],	y[3000];
	double		dtmp;
	double		sum;

	// 讀入點資訊    未來考慮要不要傳入指標
	ifstream	inx("BMP/x.txt");
	ifstream	iny("BMP/y.txt");

	count	=	0;
	while( (inx >> dtmp) != NULL )
	{
		x[ count ]	=	dtmp;
		iny >> y[ count ];
		count++;

//		if( count >= 3000 )	error_msg("variogram_fit_Gaussian");
	}

	inx.close();
	iny.close();

	// initial
	A	=	l*l;
	B	=	0;
	C	=	w*w;
	D	=	-2.0 * cx *l*l;
	E	=	-2.0 * cy *w*w;
	F	=	l*l*cx*cx + w*w*cy*cy - w*w*l*l;
	/*A	=	1;
	B	=	0;
	C	=	1;
	D	=	0;
	E	=	0;
	F	=	-10;*/

	// 設定function
	Problem problem;
	for ( i = 0; i < count; ++i ) 
		//problem.AddResidualBlock(  new AutoDiffCostFunction<FitEllipse, 1, 1, 1, 1, 1, 1 >( new FitEllipse( x[i], y[i] )  ),  NULL, &A, &B, &D, &E, &F );
		//problem.AddResidualBlock(  new AutoDiffCostFunction<FitEllipse, 1, 1, 1, 1, 1, 1 >( new FitEllipse( x[i], y[i] )  ),  NULL, &B, &C, &D, &E, &F );
		problem.AddResidualBlock(  new AutoDiffCostFunction<FitEllipse, 1, 1, 1, 1, 1, 1, 1 >( new FitEllipse( x[i], y[i] )  ),  NULL,   &A, &B, &C, &D, &E, &F );
	
	Solver::Options		options;

	options.max_num_iterations	=	MAX_ITERATOR;
	options.linear_solver_type	=	ceres::DENSE_QR;

	options.minimizer_progress_to_stdout = true;

	Solver::Summary		summary;
	Solve( options, &problem, &summary );

	std::cout << "\n" << summary.BriefReport() << "\n"; 
	
	

	// 輸出檔案  
	//ofstream	out("ellipse.txt", ios::app);用附加資料的方式
	ofstream	out("ellipse.txt");
	//out << "\n" << A << "x^2 + " << B << "xy + " << C << "y^2 + " << D << "x + " << E << "y + " << F << "\n";
	out << A << " " << B << " " << C << " " << D << " " << E << " " << F ;
	out.close();

/*
	double	a,	b,	c,	y1,	y2;
	double	Delta;
	for( double t=1; t<image.width(); t+=1 )
	{
		a		=	C;
		b		=	B*t + E;
		c		=	A*t*t + D*t + F;
		Delta	=	b*b - 4.*a*c;
		if( Delta >= 0 )
		{		
			y1		=	0.5 * ( -b + sqrt(Delta) ) / a;
			y2		=	0.5 * ( -b - sqrt(Delta) ) / a;

			image.pixel_channel( (int)t, (int)y1, 0, 255, 0 );
			image.pixel_channel( (int)t, (int)y2, 0, 255, 0 );

		}

	}
	static int	tt	=	0;
	char	str[100];
	//sprintf( str, "final_%d.bmp", tt++ );
*/

	//image.save_image(str);

	return 0;
}





/*===============================================================================================
	處理Gaussian的實做
/*===============================================================================================*/
double		circle_fit( double cx, double cy, double w, double l ) 
{

	int			i;
	int			count;
	double		h,	k,	r;
	double		x[3000],	y[3000];
	double		dtmp;
	double		sum;

	// 讀入點資訊    未來考慮要不要傳入指標
	ifstream	inx("BMP/x.txt");
	ifstream	iny("BMP/y.txt");

	count	=	0;
	while( (inx >> dtmp) != NULL )
	{
		x[ count ]	=	dtmp;
		iny >> y[ count ];
		count++;
	}
	inx.close();
	iny.close();

	/*ifstream	in("subpix.txt");
	i		=	0;
	count	=	0;
	int		id;
	while( (in >> id >> x[i] >> y[i]) != NULL )
	{
		i++;
		count++;
	}*/

	// initial
	h	=	cx;
	k	=	cy;
	r	=	w;

	// 設定function
	Problem	problem;
	for ( i = 0; i < count; ++i ) 
		problem.AddResidualBlock(  new AutoDiffCostFunction<FitCircle, 1, 1, 1, 1 >( new FitCircle( x[i], y[i] )  ),  NULL,   &h, &k, &r );
	
	Solver::Options		options;

	options.max_num_iterations	=	MAX_ITERATOR;
	options.linear_solver_type	=	ceres::DENSE_QR;

	options.initial_trust_region_radius	=	1.e3;			// 信賴區間
	options.logging_type				=	ceres::SILENT;			// 關掉itr訊息

	options.minimizer_progress_to_stdout = true;

	Solver::Summary		summary;

//Timer	t;
//t.Start();

	Solve( options, &problem, &summary );

//t.Stop();
//ofstream	tout("circle_time.txt",ios::app);
//tout << "time " << t.Elapsed() << " ms\n";
//tout.close();



	std::cout << "\n" << summary.BriefReport() << "\n"; 
	
	

	// 輸出檔案  
	ofstream	out("circle.txt", ios::app );
	out << h << "\t\t" << k << "\t\t" << r << endl;
	out.close();



	return 0;
}
