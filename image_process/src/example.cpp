#include "mrf.h"
#include "ICM.h"
#include "GCoptimization.h"
#include "MaxProdBP.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <QImage>
#include <cmath>

#define	T	100.0

const int sizeX	=	640;
const int sizeY	=	640;
const int K		=	5;

double	identity[6]	=	{0};

MRF::CostVal D[sizeX*sizeY*K];
MRF::CostVal V[K*K];
MRF::CostVal hCue[sizeX*sizeY];
MRF::CostVal vCue[sizeX*sizeY];



//#define	EEEEE	1279000
#define	EEEEE	40









EnergyFunction* generate_DataARRAY_SmoothFIXED_FUNCTION()
{
    int i, j;


    // generate function
    for (i=0; i<K; i++)
    for (j=i; j<K; j++)
    {
        V[i*K+j] = V[j*K+i] = (i == j) ? 0 : 2;
    }
    MRF::CostVal* ptr;
    for (ptr=&D[0]; ptr<&D[sizeX*sizeY*K]; ptr++) *ptr = rand() % 10;
    for (ptr=&hCue[0]; ptr<&hCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3 - 1; // negative multiplier possible
    for (ptr=&vCue[0]; ptr<&vCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3;

    // allocate eng
    DataCost *data         = new DataCost(D);
    SmoothnessCost *smooth = new SmoothnessCost(V,hCue,vCue);
    EnergyFunction *eng    = new EnergyFunction(data,smooth);

    return eng;
}

EnergyFunction* generate_DataARRAY_SmoothTRUNCATED_LINEAR()
{
    // generate function
    MRF::CostVal* ptr;
    for (ptr=&D[0]; ptr<&D[sizeX*sizeY*K]; ptr++) *ptr = rand() % 10;
    for (ptr=&hCue[0]; ptr<&hCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3;
    for (ptr=&vCue[0]; ptr<&vCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3;
    MRF::CostVal smoothMax = 5, lambda = 2;

    // allocate eng

    DataCost *data         = new DataCost(D);
    SmoothnessCost *smooth = new SmoothnessCost(1,smoothMax,lambda,hCue,vCue);
    EnergyFunction *eng    = new EnergyFunction(data,smooth);

    return eng;
}


EnergyFunction* generate_DataARRAY_SmoothTRUNCATED_QUADRATIC()
{
#if 0
    // generate function
    MRF::CostVal* ptr;
    for (ptr=&D[0]; ptr<&D[sizeX*sizeY*K]; ptr++) *ptr = rand() % 10;
    for (ptr=&hCue[0]; ptr<&hCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3;
    for (ptr=&vCue[0]; ptr<&vCue[sizeX*sizeY]; ptr++) *ptr = rand() % 3;
    MRF::CostVal smoothMax = 5,lambda=2;

    // allocate eng
    DataCost *data         = new DataCost(D);
    SmoothnessCost *smooth = new SmoothnessCost(2,smoothMax,lambda,hCue,vCue);
    EnergyFunction *eng    = new EnergyFunction(data,smooth);

    return eng;
#endif
    // generate function
    MRF::CostVal* ptr;
	int		i,	j,	label;
	double	value;
	QImage	image("../DATA/denoise_1_test3.bmp");
    
	// generate D
	for( i = 0; i < sizeX; i++ )
		for( j = 0; j < sizeY; j++ )
			for( label = 0; label < K; label++ )
			{
				value	=	image.gray_channel(i,j);

				switch(label)
				{
					case 0:
						D[ (i*sizeY + j)*K + label ]	=	pow( value-5.65022, 2 ) / (2.0*pow(4.90117,2));
						break;
					case 1:
						D[ (i*sizeY + j)*K + label ]	=	pow( value-248.107, 2 ) / (2.0*pow(5.25327,2));
						break;
					case 2:
						D[ (i*sizeY + j)*K + label ]	=	pow( value-49.0112, 2 ) / (2.0*pow(9.04484,2));
						break;
					case 3:
						D[ (i*sizeY + j)*K + label ]	=	pow( value-124.013, 2 ) / (2.0*pow(9.0423,2));
						break;
					case 4:
						D[ (i*sizeY + j)*K + label ]	=	pow( value-133.994, 2 ) / (2.0*pow(9.1176,2));
						break;
					default:
						assert(false);
				}
			}
	// generate hCu
    for (ptr=&hCue[0]; ptr<&hCue[sizeX*sizeY]; ptr++) *ptr = 1.0;//rand() % 3;
    for (ptr=&vCue[0]; ptr<&vCue[sizeX*sizeY]; ptr++) *ptr = 1.0;//rand() % 3;
    MRF::CostVal smoothMax = 1000,lambda=0.5;

    // allocate eng
    DataCost *data         = new DataCost(D);
    SmoothnessCost *smooth = new SmoothnessCost(2,smoothMax,lambda,hCue,vCue);
    EnergyFunction *eng    = new EnergyFunction(data,smooth);



	return eng;
}


MRF::CostVal dCost(int pix, int i)
{
    return (pix*i + i + pix) % 10;
}

MRF::CostVal fnCost(int pix1, int pix2, int i, int j)
{
    return (pix1*(i+1)*(j+2) + pix2*i*j*pix1 - 2*i*j*pix1) % 10;
}


EnergyFunction* generate_DataFUNCTION_SmoothGENERAL_FUNCTION()
{
    DataCost *data         = new DataCost(dCost);
    SmoothnessCost *smooth = new SmoothnessCost(fnCost);
    EnergyFunction *eng    = new EnergyFunction(data,smooth);

    return eng;
}

int f()
{
	MRF* mrf;
    EnergyFunction *eng;
    MRF::EnergyVal E;
    float t,tot_t;
    int iter;

    int seed = 1124285485;
    srand(seed);

    // There are 4 sample energies below to play with. Uncomment 1 at a time 

    //eng = generate_DataARRAY_SmoothFIXED_FUNCTION();
    //eng = generate_DataARRAY_SmoothTRUNCATED_LINEAR();
    eng = generate_DataARRAY_SmoothTRUNCATED_QUADRATIC();
    //eng = generate_DataFUNCTION_SmoothGENERAL_FUNCTION();



    ////////////////////////////////////////////////
    //                     ICM                    //
    ////////////////////////////////////////////////
#if 1
    printf("\n*******Started ICM *****\n");

    mrf = new ICM(sizeX,sizeY,K,eng);
    mrf->initialize();
    mrf->clearAnswer();
    
    E = mrf->totalEnergy();
    printf("Energy at the Start= %lf (%lf,%lf)\n", E,mrf->smoothnessEnergy(),mrf->dataEnergy());

    tot_t = 0;
    for (iter=0; iter<1; iter++)
    {
        mrf->optimize(100, t);

		((ICM*)mrf)->save_ans();

        E = mrf->totalEnergy();
        tot_t = tot_t + t ;
        printf("energy = %lf (%f secs)\n", E, tot_t);
    }

    delete mrf;
#endif

    ////////////////////////////////////////////////
    //          Graph-cuts expansion              //
    ////////////////////////////////////////////////
#if 1
    printf("\n*******Started the graph-cuts expansion *****\n");
    mrf = new Expansion(sizeX,sizeY,K,eng);
    mrf->initialize();
    mrf->clearAnswer();
    
    E = mrf->totalEnergy();
    printf("Energy at the Start= %lf (%lf,%lf)\n", E,mrf->smoothnessEnergy(),mrf->dataEnergy());

    tot_t = 0;
    for (iter=0; iter<1; iter++)
    {
        mrf->optimize(100, t);

		((Expansion*)mrf)->save_ans();

        E = mrf->totalEnergy();
        tot_t = tot_t + t ;
        printf("energy = %lf (%f secs)\n", E, tot_t);
    }

    delete mrf;
#endif


    ////////////////////////////////////////////////
    //          Graph-cuts swap                   //
    ////////////////////////////////////////////////
#if 1
    printf("\n*******Started the graph-cuts swap *****\n");
    mrf = new Swap(sizeX,sizeY,K,eng);
    mrf->initialize();
    mrf->clearAnswer();
    
    E = mrf->totalEnergy();
    printf("Energy at the Start= %lf (%lf,%lf)\n", E,mrf->smoothnessEnergy(),mrf->dataEnergy());

    tot_t = 0;
    for (iter=0; iter<1; iter++)
    {
        mrf->optimize(100, t);

		((Swap*)mrf)->save_ans();

        E = mrf->totalEnergy();
        tot_t = tot_t + t ;
        printf("energy = %d (%f secs)\n", E, tot_t);
    }

    
    delete mrf;
#endif

    ////////////////////////////////////////////////
    //          Belief Propagation                //
    ////////////////////////////////////////////////

#if 1

    printf("\n*******  Started MaxProd Belief Propagation *****\n");
    mrf = new MaxProdBP(sizeX,sizeY,K,eng);
    mrf->initialize();
    mrf->clearAnswer();
    
    E = mrf->totalEnergy();
    printf("Energy at the Start= %lf (%d,%d)\n", E,mrf->smoothnessEnergy(),mrf->dataEnergy());

    tot_t = 0;
    for (iter=0; iter < 1; iter++)
    {
        mrf->optimize(100, t);

		((MaxProdBP*)mrf)->save_ans();

        E = mrf->totalEnergy();
        tot_t = tot_t + t ;
        printf("energy = %lf (%f secs)\n", E, tot_t);
    }

    
    delete mrf;

#endif

    return 0;
}

