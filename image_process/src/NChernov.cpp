#include "stdafx.h"
#include "NChernov.h"

using namespace NChernov;
using namespace std;








//   next define some frequently used functions:
template<typename T>
inline T SQR(T t) { return t*t; };

reals NChernov::pythag(reals a, reals b)
{
	reals absa=abs(a),	absb=abs(b);
	if (absa > absb) return absa*sqrt(One+SQR(absb/absa));
	else return (absb == 0.0 ? 0.0 : absb*sqrt(One+SQR(absa/absb)));
}

bool	NChernov::isfinite(reals a)
{
	if( a >= REAL_MAX )
		return	true;
	else
		return	false;
}


void NChernov::RandomNormalPair( reals& x, reals& y )
{
    reals rand1,rand2,wrand;

    do {
         rand1 = Two*rand()/RAND_MAX - One;
         rand2 = Two*rand()/RAND_MAX - One;
         wrand = rand1*rand1 + rand2*rand2;
       } while (wrand >= One);
    wrand = sqrt( (-Two*log(wrand) ) / wrand );
    x = rand1 * wrand;
    y = rand2 * wrand;

}

//*********************** SimulateArc ******************************
void NChernov::SimulateArc(Data& data, reals a, reals b, reals R, reals theta1, reals theta2, reals sigma)
{
	int N=data.n; reals theta,dx,dy;
	
	for (int i=0; i<N; i++)
	{
		theta = theta1 + (theta2 - theta1)*i/(N-1);
		RandomNormalPair(dx,dy);
		data.X[i] = a + R*cos(theta) + sigma*dx;
		data.Y[i] = b + R*sin(theta) + sigma*dy;
	}
}

//********************* SimulateRandom ****************************
void NChernov::SimulateRandom(Data& data, reals Window)
{
	for (int i=0; i<data.n; i++)
	{
		data.X[i] = Window*(Two*rand()/RAND_MAX - One);
		data.Y[i] = Window*(Two*rand()/RAND_MAX - One);
	}
}

//****************** Sigma ************************************
reals NChernov::Sigma (Data& data, Circle& circle)
{
    reals sum = 0.,dx,dy;

    for (int i=0; i<data.n; i++)
    {
        dx = data.X[i] - circle.a;
        dy = data.Y[i] - circle.b;
        sum += SQR(sqrt(dx*dx+dy*dy) - circle.r);
    }
    return sqrt(sum/data.n);
}


//****************** SigmaReduced ************************************
reals NChernov::SigmaReduced (Data& data, Circle& circle)
{
    int i,n=data.n;
    reals sum=0.,dx,dy,r;//,D[n];
	reals	*D	=	new reals[n];

    for (i=0; i<n; i++)
    {
        dx = data.X[i] - circle.a;
        dy = data.Y[i] - circle.b;
        D[i] = sqrt(dx*dx+dy*dy);
        sum += D[i];
    }
    r = sum/n;

    for (sum=0., i=0; i<n; i++)  sum += SQR(D[i] - r);
    
    return sqrt(sum/n);
}

//****************** OptimalRadius ******************************
reals NChernov::OptimalRadius (Data& data, Circle& circle)
{
    reals Mr = 0.,dx,dy;

    for (int i=0; i<data.n; i++)
    {
        dx = data.X[i] - circle.a;
        dy = data.Y[i] - circle.b;
        Mr += sqrt(dx*dx + dy*dy);
    }
    return Mr/data.n;
}







//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~這邊是Data結構~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/************************************************************************
			BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor
Data::Data()
{
	n=0;
	X = new reals[n];
	Y = new reals[n];
	for (int i=0; i<n; i++)
	{
		X[i]=0.;
		Y[i]=0.;
	}
}

// Constructor with assignment of the field N
Data::Data(int N)
{
	n=N;
	X = new reals[n];
	Y = new reals[n];

	for (int i=0; i<n; i++)
	{
		X[i]=0.;
		Y[i]=0.;
	}
}

// Constructor with assignment of each field
Data::Data(int N, reals dataX[], reals dataY[])
{
	n=N;
	X = new reals[n];
	Y = new reals[n];

	for (int i=0; i<n; i++)
	{
		X[i]=dataX[i];
		Y[i]=dataY[i];
	}
}

// Routine that computes the x- and y- sample means (the coordinates of the centeroid)

void Data::means(void)
{
	meanX=0.; meanY=0.;
	
	for (int i=0; i<n; i++)
	{
		meanX += X[i];
		meanY += Y[i];
	}
	meanX /= n;
	meanY /= n;
}

// Routine that centers the data set (shifts the coordinates to the centeroid)

void Data::center(void)
{
	reals sX=0.,sY=0.;  
	int i;
	
	for (i=0; i<n; i++)
	{
		sX += X[i];
		sY += Y[i];
	}
	sX /= n;
	sY /= n;
	
	for (i=0; i<n; i++)
	{
		X[i] -= sX;
		Y[i] -= sY;
	}
	meanX = 0.;
	meanY = 0.;
}

// Routine that scales the coordinates (makes them of order one)

void Data::scale(void)
{
	reals sXX=0.,sYY=0.,scaling;  
	int i;
	
	for (i=0; i<n; i++)
	{
		sXX += X[i]*X[i];
		sYY += Y[i]*Y[i];
	}
	scaling = sqrt((sXX+sYY)/n/Two);
	
	for (i=0; i<n; i++)
	{
		X[i] /= scaling;
		Y[i] /= scaling;
	}
}

// Printing routine

void Data::print(void)
{
	cout << endl << "The data set has " << n << " points with coordinates :"<< endl;
	
	for (int i=0; i<n-1; i++) cout << "(" << X[i] << ","<< Y[i] << "), ";
	
	cout << "(" << X[n-1] << ","<< Y[n-1] << ")\n";
}

// Destructor
Data::~Data()
{
	// TO IMPLEMENT 
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~這邊是circle結構~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/************************************************************************
			BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor

Circle::Circle()
{
	a=0.; b=0.; r=1.; s=0.; i=0; j=0;
}

// Constructor with assignment of the circle parameters only

Circle::Circle(reals aa, reals bb, reals rr)
{
	a=aa; b=bb; r=rr;
}

// Printing routine

void Circle::print(void)
{
	/*cout << endl;
	cout << setprecision(10) << "center (" <<a <<","<< b <<")  radius "
		 << r << "  sigma " << s << "  gradient " << g << "  iter "<< i << "  inner " << j << endl;*/

	ofstream	out("circle_taubin.txt",ios::app);
	out << a << "\t\t" << b << "\t\t" << r << endl;
	out.close();

}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~各個fitting~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Circle NChernov::CircleFitByCL (Data& data, Circle& Ini, reals LambdaIni) 
{
    int i,iter,IterMAX=99,inner;
    
    reals factorUp=10.,factorDown=0.04,lambda;
    reals Aold,Fold,Told,Anew,Fnew,Tnew,DD,H,aabb;
    reals Xi,Yi,Zi,Ui,Vi,Gi,CT,ST,D,ADF,SQ,DEN,FACT,DGDAi,DGDFi,DGDTi;
    reals H11,H12,H13,H22,H23,H33,F1,F2,F3,GG,dA,dF,dT;
    reals epsilon=3.e-8;  
    reals G11,G22,G33,G12,G13,G23,D1,D2,D3;
    reals Xshift=0,Yshift=0.,dX=One,dY=0.,aTemp,bTemp,rTemp;
    
    Circle Old,New;
    
    //    starting with the given initial guess
    
    New = Ini;
    //New.r = OptimalRadius(data,New);
    New.s = Sigma(data,New);
    
    Anew = One/Two/New.r;
    aabb = New.a*New.a + New.b*New.b;
    Fnew = (aabb - New.r*New.r)*Anew;
    Tnew = acos(-New.a/sqrt(aabb));
    if (New.b > 0.) Tnew = Two*Pi - Tnew;
    
    if (One+Four*Anew*Fnew < epsilon) 
    {
        //if (lpr==2) cout << "     initial violation: " << One+Four*Anew*Fnew << endl;
        Xshift += dX;
        Yshift += dY;
        
        New.a += dX;
        New.b += dY;
        aabb = New.a*New.a + New.b*New.b;
        Fnew = (aabb - New.r*New.r)*Anew;
        Tnew = acos(-New.a/sqrt(aabb));
        if (New.b > 0.) Tnew = Two*Pi - Tnew;
    }
    
    //    initializing lambda and iter
    
    lambda = LambdaIni;
    iter = inner = 0;
    
NextIteration:
    
    Aold = Anew;
    Fold = Fnew;
    Told = Tnew;
    Old = New;
    
    if (++iter > IterMAX) goto enough;
    
    //    computing moments
    
shiftXY:
	
    DD = One + Four*Aold*Fold;
    D = sqrt(DD);
    CT = cos(Told);
    ST = sin(Told);
    
    H11=H12=H13=H22=H23=H33=F1=F2=F3=0.;
    
    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] + Xshift;
        Yi = data.Y[i] + Yshift;
        Zi = Xi*Xi + Yi*Yi;
        Ui = Xi*CT + Yi*ST;
        Vi =-Xi*ST + Yi*CT;
        
        ADF = Aold*Zi + D*Ui + Fold;
        SQ = sqrt(Four*Aold*ADF + One);
        DEN = SQ + One;
        Gi = Two*ADF/DEN;
        FACT = Two/DEN*(One - Aold*Gi/SQ);
        DGDAi = FACT*(Zi + Two*Fold*Ui/D) - Gi*Gi/SQ;
        DGDFi = FACT*(Two*Aold*Ui/D + One);
        DGDTi = FACT*D*Vi;
        
        H11 += DGDAi*DGDAi;
        H12 += DGDAi*DGDFi;
        H13 += DGDAi*DGDTi;
        H22 += DGDFi*DGDFi;
        H23 += DGDFi*DGDTi;
        H33 += DGDTi*DGDTi;
        
        F1 += Gi*DGDAi;
        F2 += Gi*DGDFi;
        F3 += Gi*DGDTi;
    }
    Old.g = New.g = sqrt(F1*F1 + F2*F2 + F3*F3);
    
try_again:
	
	//      Cholesky decomposition
	
    G11 = sqrt(H11 + lambda);
    G12 = H12/G11;
    G13 = H13/G11;
    G22 = sqrt(H22 + lambda - G12*G12);
    G23 = (H23 - G12*G13)/G22;
    G33 = sqrt(H33 + lambda - G13*G13 - G23*G23);
    
    D1 = F1/G11;
    D2 = (F2 - G12*D1)/G22;
    D3 = (F3 - G13*D1 - G23*D2)/G33;
    
    dT = D3/G33;
    dF = (D2 - G23*dT)/G22;
    dA = (D1 - G12*dF - G13*dT)/G11;
    
    //    updating the parameters
    
    Anew = Aold - dA;
    Fnew = Fold - dF;
    Tnew = Told - dT;
    
    if (One+Four*Anew*Fnew < epsilon) 
    {
        Xshift += dX;
        Yshift += dY;
        
        H = sqrt(One+Four*Aold*Fold);
        aTemp = -H*cos(Told)/(Aold+Aold) + dX;
        bTemp = -H*sin(Told)/(Aold+Aold) + dY;
        rTemp = One/abs(Aold+Aold);
        
        Aold = One/(rTemp + rTemp);
        aabb = aTemp*aTemp + bTemp*bTemp;
        Fold = (aabb - rTemp*rTemp)*Aold;
        Told = acos(-aTemp/sqrt(aabb));
        if (bTemp > 0.) Told = Two*Pi - Told;
        
        //if (lpr==2) cout << "     current violation: " << One+Four*Anew*Fnew << endl;
        lambda *= factorUp;
        inner++;
        goto shiftXY;
    }
    
    H = sqrt(One+Four*Anew*Fnew);
    New.a = -H*cos(Tnew)/(Anew+Anew) - Xshift;
    New.b = -H*sin(Tnew)/(Anew+Anew) - Yshift;
    New.r = One/abs(Anew+Anew);
    //New.r = OptimalRadius(data,New);
    New.s = Sigma(data,New);
    
    if ((abs(New.a-Old.a) + abs(New.b-Old.b) + abs(New.r-Old.r))/(One + Old.r) < epsilon) goto enough;
    
    //    checking if improvement is gained
    
    if (New.s < Old.s)    //   yes, improvement
    {
    	lambda *= factorDown;
        goto NextIteration;
    }
    else                       //   no improvement
    {
        if (++inner > IterMAX) goto enough;
        lambda *= factorUp;
        goto try_again;
    }
    
    //       exit
    
enough:
	
    Old.i = iter;
    Old.j = inner;
    
    return Old;
}

//****************** Perturb *********************************
Circle NChernov::Perturb (Circle& New, Circle& Old, reals range)
{
    Circle Perturbed;

    if (range==0.) return New;

    Perturbed.a = New.a + (New.a - Old.a)*(range*rand()/RAND_MAX - range/Two);
    Perturbed.b = New.b + (New.b - Old.b)*(range*rand()/RAND_MAX - range/Two);
    Perturbed.r = New.r + (New.r - Old.r)*(range*rand()/RAND_MAX - range/Two);

    return Perturbed;
}



/****************** CircleFitByHyper *************************************
  
     Hyper ("hyperaccurate") circle fit. It is an algebraic circle fit 
       with "hyperaccuracy" (with zero essential bias)

      A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
      Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)

     Input:  data is a class object containing the coordinates of the data points
             (see description in the file data.h)
     
     Output: circle is a class object containing the parameters of the fitting circle
             (see description in the file circle.h)

     This method combines the Pratt and Taubin fits to eliminate the essential bias.
        
     It works well whether data points are sampled along an entire circle or
     along a small arc. 
     
     Its statistical accuracy is slightly higher than that of the geometric fit 
     (minimizing geometric distances) and higher than that of the Pratt fit 
     and Taubin fit. 
     
     It provides a very good initial guess for a subsequent geometric fit. 
     
       Nikolai Chernov  (September 2012)

**************************************************************************/
void NChernov::CircleFitByHyper (Data& data, Circle& circle)
//                     <- input -> <-- output -->
{
    int i,iter,IterMAX=99;

    reals Xi,Yi,Zi;
    reals Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    reals A0,A1,A2,A22;
    reals Dy,xnew,x,ynew,y;
    reals DET,Xcenter,Ycenter;

    data.means();   // Compute x- and y- sample means (via a function in the class "data") 

//     computing moments 
//             (note: all moments will be normed, i.e. divided by the number of points)

    Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;

//    computing the coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;

    A2 = Four*Cov_xy - Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0  
//     (it is guaranteed to converge to the right root)

	for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
        Dy = A1 + x*(A22 + 16.*x*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + Four*xnew*xnew));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }

//    computing the circle parameters

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;

    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz - x - x);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too
}


/****************** CircleFitByKasa **********************************
  
     Simple circle fit (Kasa method)
      I. Kasa, "A curve fitting procedure and its error analysis",
      IEEE Trans. Inst. Meas., Vol. 25, pages 8-14, (1976)

     Input:  data is a class object containing the coordinates of the data points
             (see description in the file data.h)
     
     Output: circle is a class object containing the parameters of the fitting circle
             (see description in the file circle.h)
             
     The method is based on the minimization of the function
     
                 F = sum [(x-a)^2 + (y-b)^2 - R^2]^2
                 
     This is perhaps the simplest and fastest circle fit. 
     
     It works well when data points are sampled along the entire circle
     or a large part of it (at least half circle).
     
     It does not work well when data points are sampled along a small arc 
     of the circle. In that case the meathod is heavily biased, it returns
     circles that are often too small.

     It is the oldest algebraic circle fit (first published in 1972?).
     For 20-30 years it has been the most popular circle fit, at least
     until the more robust Pratt fit (1987) and Taubin fit (1991) were invented.
     
       Nikolai Chernov  (September 2012)

*********************************************************************/

void NChernov::CircleFitByKasa (Data& data, Circle& circle)
//                    <- input -> <-- output -->
{
    int i;

    reals Xi,Yi,Zi;
    reals Mxy,Mxx,Myy,Mxz,Myz;
    reals B,C,G11,G12,G22,D1,D2;

    data.means();   // Compute x- and y- sample means (via a function in the class "data") 

//     computing moments 
//             (note: all moments will be normed, i.e. divided by the number of points)

    Mxx=Myy=Mxy=Mxz=Myz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates

        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxy += Xi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;

//    solving system of equations by Cholesky factorization

    G11 = sqrt(Mxx);
    G12 = Mxy/G11;
    G22 = sqrt(Myy - G12*G12);

    D1 = Mxz/G11;
    D2 = (Myz - D1*G12)/G22;

//    computing the circle parameters

    C = D2/G22/Two;
    B = (D1 - G12*C)/G11/Two;

    circle.a = B + data.meanX;
    circle.b = C + data.meanY;
    circle.r = sqrt(B*B + C*C + Mxx + Myy);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = 0;
}





//****************** CircleLMF *****************************
//
//       Levenberg-Marquard algorithm for a,b,R (Full)

void NChernov::CircleFitByLMFull (Data& data, Circle& circleIni, reals LambdaIni, Circle& circle)
{
    int i,iter,inner,IterMAX=99;
    
    reals factorUp=10.,factorDown=0.04,lambda,ParLimit=1.e+6;
    reals dx,dy,ri,u,v;
    reals Mu,Mv,Muu,Mvv,Muv,Mr,UUl,VVl,Nl,F1,F2,F3,dX,dY,dR;
    reals epsilon=3.e-8;
    reals G11,G22,G33,G12,G13,G23,D1,D2,D3;
    
    Circle Old,New;
    
    //    starting with the given initial guess
    
    New = circleIni;
    New.s = Sigma(data,New);
    
    //    initializing lambda and iter
    
    lambda = LambdaIni;
    iter = inner = 0;
    
NextIteration:
	
    Old = New;
    if (++iter > IterMAX) goto enough;
    
    //    computing moments
    
    Mu=Mv=Muu=Mvv=Muv=Mr=0.;
    
    for (i=0; i<data.n; i++)
    {
        dx = data.X[i] - Old.a;
        dy = data.Y[i] - Old.b;
        ri = sqrt(dx*dx + dy*dy);
        u = dx/ri;
        v = dy/ri;
        Mu += u;
        Mv += v;
        Muu += u*u;
        Mvv += v*v;
        Muv += u*v;
        Mr += ri;
    }
    Mu /= data.n;
    Mv /= data.n;
    Muu /= data.n;
    Mvv /= data.n;
    Muv /= data.n;
    Mr /= data.n;
    
    //    computing matrices
    
    F1 = Old.a + Old.r*Mu - data.meanX;
    F2 = Old.b + Old.r*Mv - data.meanY;
    F3 = Old.r - Mr;
    
    Old.g = New.g = sqrt(F1*F1 + F2*F2 + F3*F3);
    
try_again:
	
    UUl = Muu + lambda;
    VVl = Mvv + lambda;
    Nl = One + lambda;
    
    //      Cholesly decomposition
    
    G11 = sqrt(UUl);
    G12 = Muv/G11;
    G13 = Mu/G11;
    G22 = sqrt(VVl - G12*G12);
    G23 = (Mv - G12*G13)/G22;
    G33 = sqrt(Nl - G13*G13 - G23*G23);
    
    D1 = F1/G11;
    D2 = (F2 - G12*D1)/G22;
    D3 = (F3 - G13*D1 - G23*D2)/G33;
    
    dR = D3/G33;
    dY = (D2 - G23*dR)/G22;
    dX = (D1 - G12*dY - G13*dR)/G11;
    
    if ((abs(dR)+abs(dX)+abs(dY))/(One+Old.r) < epsilon) goto enough;
    
    //    updating the parameters
    
    New.a = Old.a - dX;
    New.b = Old.b - dY;
    
    if (abs(New.a)>ParLimit || abs(New.b)>ParLimit) goto enough;
    
    New.r = Old.r - dR;
    
    if (New.r <= 0.)
    {
        lambda *= factorUp;
        if (++inner > IterMAX) goto enough;
        goto try_again;
    }
    
    New.s = Sigma(data,New);
    
    //if (lpr==2) cout << iter <<"  ("<<New.a<<", "<<New.b<<")  "<<New.r<<"  "<<New.s<<endl;
    
    //    checking if improvement is gained
    
    if (New.s < Old.s)    //   yes, improvement
    {
        lambda *= factorDown;
        goto NextIteration;
    }
    else                       //   no improvement
    {
        if (++inner > IterMAX) goto enough;
        lambda *= factorUp;
        goto try_again;
    }
    
    //       exit
    
enough:
	
    Old.i = iter;
    Old.j = inner;
    
    circle = Old;
}





//****************** CircleLMR *****************************
//
//       Levenberg-Marquard algorithm for a,b (Reduced)
void NChernov::CircleFitByLMReduced (Data& data, Circle& circleIni, reals LambdaIni, Circle& circle)
{
    int i,pivot,iter,IterMAX=99,inner;

    reals factorUp=10.,factorDown=0.04,lambda,ParLimit=1.e+6;
    reals dx,dy,ri,u,v;
    reals Mu,Mv,Muu,Mvv,Muv,Mr,A11,A12,A22,F1,F2,dX,dY;
    reals epsilon=3.e-8;
    reals G11,G12,G22,D1,D2;

    Circle Old,New;

    data.means();   // Compute x- and y-means (via a function in class "data") 

//    starting with the given initial guess

    New = circleIni;
    New.s = SigmaReduced(data,New);
    /*if (lpr==2) */cout << "in  ("<<New.a<<","<<New.b<<")  r="<<New.r<<"  s="<<New.s<<"  L="<<lambda<<endl;

//    initializing lambda and iter

    lambda = LambdaIni;
    iter = inner = 0;

NextIteration:

    Old = New;
    if (++iter > IterMAX) goto enough;

//    computing moments

    Mu=Mv=Muu=Mvv=Muv=Mr=0.;

    for (i=0; i<data.n; i++)
    {
        dx = data.X[i] - Old.a;
        dy = data.Y[i] - Old.b;
        ri = sqrt(dx*dx + dy*dy);
        u = dx/ri;
        v = dy/ri;
        Mu += u;
        Mv += v;
        Muu += u*u;
        Mvv += v*v;
        Muv += u*v;
        Mr += ri;
    }
    Mu /= data.n;
    Mv /= data.n;
    Muu /= data.n;
    Mvv /= data.n;
    Muv /= data.n;
    Mr /= data.n;

//    computing matrices

    F1 = Old.a + Mu*Mr - data.meanX;
    F2 = Old.b + Mv*Mr - data.meanY;
    
    Old.g = New.g = sqrt(F1*F1 + F2*F2);

try_again:

    A11 = (Muu - Mu*Mu) + lambda;
    A22 = (Mvv - Mv*Mv) + lambda;
    A12 = Muv - Mu*Mv;

    if (A11<epsilon || A22<epsilon || A11*A22-A12*A12<epsilon)
    {
        lambda *= factorUp;
        goto try_again;
    }

//      Cholesly decomposition with pivoting

    pivot=0;
    if (A11 < A22)
    {
       	swap(A11,A22);
    	    swap(F1,F2);
        pivot=1;
    }

    G11 = sqrt(A11);
    G12 = A12/G11;
    G22 = sqrt(A22 - G12*G12);

    D1 = F1/G11;
    D2 = (F2 - G12*D1)/G22;

//    updating the parameters

    dY = D2/G22;
    dX = (D1 - G12*dY)/G11;
    
    if ((abs(dX)+abs(dY))/(One+abs(Old.a)+abs(Old.b)) < epsilon) goto enough;

    if (pivot != 0) swap(dX,dY);

    New.a = Old.a - dX;
    New.b = Old.b - dY;

    if (abs(New.a)>ParLimit || abs(New.b)>ParLimit) goto enough;

    New.s = SigmaReduced(data,New);
   /* if (lpr==2)*/ cout << iter <<"  ("<<New.a<<","<<New.b<<")  r="<<New.r<<"  s="<<New.s<<"  L="<<lambda<<endl;

//    checking if improvement is gained

    if (New.s < Old.s)    //    yes, improvement
    {
        lambda *= factorDown;
        goto NextIteration;
    }
    else                  //  no improvement
    {
        if (++inner > IterMAX) goto enough;
        lambda *= factorUp;
        goto try_again;
    }

//       exit

enough:

    Old.r = OptimalRadius(data,Old);
    Old.i = iter;
    Old.j = inner;
    circle = Old;
}




/****************** CircleFitByPratt *************************************
  
     Algebraic circle fit by Pratt (using Newton's method)
      V. Pratt, "Direct least-squares fitting of algebraic surfaces",
      Computer Graphics, Vol. 21, pages 145-152 (1987)

     Input:  data is a class object containing the coordinates of the data points
             (see description in the file data.h)
     
     Output: circle is a class object containing the parameters of the fitting circle
             (see description in the file circle.h)
             
     The method is based on the minimization of the function
     
                 F = sum [(x-a)^2 + (y-b)^2 - R^2]^2 / R^2
                 
     This method is more balanced than the simple Kasa fit.
        
     It works well whether data points are sampled along an entire circle or
     along a small arc. 
     
     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances).
     
     It provides a good initial guess for a subsequent geometric fit. 
     
       Nikolai Chernov  (September 2012)

**************************************************************************/

void NChernov::CircleFitByPratt (Data& data, Circle& circle)
//                     <- input -> <-- output -->
{
    int i,iter,IterMAX=99;

    reals Xi,Yi,Zi;
    reals Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    reals A0,A1,A2,A22;
    reals Dy,xnew,x,ynew,y;
    reals DET,Xcenter,Ycenter;

    data.means();   // Compute x- and y- sample means (via a function in the class "data") 

//     computing moments 
//             (note: all moments will be normed, i.e. divided by the number of points)

    Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;

//    computing the coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;

    A2 = Four*Cov_xy - Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0  
//     (it is guaranteed to converge to the right root)

    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
        Dy = A1 + x*(A22 + 16.*x*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + Four*xnew*xnew));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }

//    computing the circle parameters

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;

    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz + x + x);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too
}




/****************** CircleFitByTaubin *************************************
  
     Circle fit by Taubin
      G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
                  Space Curves Defined By Implicit Equations, With 
                  Applications To Edge And Range Image Segmentation",
      IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)

     Input:  data is a class object containing the coordinates of the data points
             (see description in the file data.h)
     
     Output: circle is a class object containing the parameters of the fitting circle
             (see description in the file circle.h)
             
     The method is based on the minimization of the function
     
                  sum [(x-a)^2 + (y-b)^2 - R^2]^2
              F = -------------------------------
                      sum [(x-a)^2 + (y-b)^2]
                 
     This method is more balanced than the simple Kasa fit.
        
     It works well whether data points are sampled along an entire circle or
     along a small arc. 
     
     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances),
     but slightly higher than that of the very similar Pratt fit. 
     Besides, the Taubin fit is slightly simpler than the Pratt fit
     
     It provides a very good initial guess for a subsequent geometric fit. 
     
       Nikolai Chernov  (September 2012)

**************************************************************************/

void NChernov::CircleFitByTaubin (Data& data, Circle& circle)
//                      <- input -> <-- output -->
{
    int i,iter,IterMAX=1000;
    
    reals Xi,Yi,Zi;
    reals Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    reals A0,A1,A2,A22,A3,A33;
    reals Dy,xnew,x,ynew,y;
    reals DET,Xcenter,Ycenter;
    
    data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments 
//             (note: all moments will be normed, i.e. divided by the number of points)

	Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;
    
    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;
        
        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;
    
    //    computing the coefficients of the characteristic polynomial
    
    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;
    A3 = Four*Mz;
    A2 = -Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;
    A33 = A3 + A3 + A3;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0  
//     (it is guaranteed to converge to the right root)
    
    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
    	    Dy = A1 + x*(A22 + A33*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + xnew*A3));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }
     
    //    computing the circle parameters
    
    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;
    
    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too
}

