
#include "OCR_read_data.h"
#include "LWOCR.h"

/******************************************************************************
	OCR READ_SCALE_TABLE
*******************************************************************************/
void	OCR_read_SCALE_TABLE()
{
	int i;
	FILE *p;
	p	=	fopen( "..\\DATA\\Scale_Table.txt", "r" );
	if(p == NULL)
	{
		printf("....READ Scale_Table, FILE CAN NOT BE OPEN!!!...");
	}
	for( i = 1; i <= 10000; i++ )
	{
		if( fscanf( p, "%d", Scale_Table + i ) == EOF )
		{
			printf("...Scale_Table LOADING ERROR!!!...");
		}
	}
	fclose(p);

}


/******************************************************************************
	讀取 SVM 資料
*******************************************************************************/
void	OCR_read_SVM_model( OCR_INIT_TYPE init_type )
{
	FILE	*p;
	int		i,	j;

	if( init_type == OCR_CNY_20 )
		p	=	fopen( _SVM_FILE_CNY_20_MODEL_ , "rb" );
	else if( init_type == OCR_CNY_100 )
		p	=	fopen( _SVM_FILE_CNY_100_MODEL_ , "rb" );
	else
	{
		// error
	}

	if( p == NULL )
		printf("error");

#ifdef _OCR_USE_FLOAT_
	Omega	=	malloc( sizeof(float)*(nr_class*nr_feature) );
#else
	Omega	=	malloc( sizeof(short) * (nr_class*nr_feature) );
#endif

	for( i = 0; i < nr_class; i++ )
		for( j = 0; j < nr_feature; j++ )
			fread( Omega + i*nr_feature + j, sizeof(short), 1, p );

	fclose(p);
}





/******************************************************************************
	預先讀取svm資料
*******************************************************************************/
void	OCR_pre_read_SVM_model( OCR_INIT_TYPE init_type )
{
	FILE	*p;
	int		i;

	if( init_type == OCR_CNY_20 )
		p	=	fopen( _SVM_FILE_CNY_20_HEAD_ , "rb" );
	else if( init_type == OCR_CNY_100 )
		p	=	fopen( _SVM_FILE_CNY_100_HEAD_ , "rb" );
	else
	{
		// error...
	}

	if( p == NULL )
		printf("error...");

	fread( &nr_class, sizeof(short), 1, p );
	fread( &nr_feature, sizeof(short), 1, p );

	O_label	=	malloc( sizeof(short) * nr_class );

	for( i = 0; i < nr_class; i++ )
		fread( O_label + i, sizeof(short), 1, p );

	fclose(p);
}


#if 0
/******************************************************************************
	將 model檔案轉成 binary
*******************************************************************************/
void		OCR_model_to_binary( OCR_INIT_TYPE init_type )
{
	FILE	*p,	*q;
	short	tmp;

	p	=	fopen("../DATA/HLF_CNY100_m9_SVM_WEIGHT.txt", "r" );
	q	=	fopen("../DATA/CNY100_model_binary.data", "wb+" );

	if( p == NULL )
		printf("error");
	if( q == NULL )
		printf("error");

	while( fscanf( p, "%hd", &tmp ) != EOF )
	{
		fwrite( &tmp, sizeof(short), 1, q );
	}

	fclose(p);
	fclose(q);
}
#endif


#if 0
void		OCR_test()
{
	/*FILE	*p;
	short	tmp;

	p	=	fopen("../DATA/CNY100_model_binary.data", "rb" );

	if( p == NULL )
		printf("error");


	while( fread( &tmp, sizeof(short), 1, p ) != NULL )
	{
		printf( "%hd ", tmp );
	}

	fclose(p);*/

	/*FILE	*p;
	short	buf;
	short	label[33]	=	{28, 29, 30, 31, 32, 33, 34, 35, 1 ,2, 3, 4, 5, 6, 7, 8, 9 ,10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26};

	p	=	fopen(_SVM_FILE_CNY_20_HEAD_, "wb+");

	buf	=	33;		// nr_class
	fwrite( &buf, sizeof(short), 1, p );

	buf	=	129;	// nr_feature
	fwrite( &buf, sizeof(short), 1, p );

	fwrite( label, sizeof(short), 33, p );*/


	FILE	*p;
	short	buf;
	short	label[34]	=	{27, 28 ,29 ,30, 31, 32, 33, 34, 35, 1 ,2 ,3, 4, 5, 6, 7 ,8 ,9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26};

	p	=	fopen( _SVM_FILE_CNY_100_HEAD_ , "wb+");

	buf	=	34;		// nr_class
	fwrite( &buf, sizeof(short), 1, p );

	buf	=	129;	// nr_feature
	fwrite( &buf, sizeof(short), 1, p );

	fwrite( label, sizeof(short), 34, p );

	/*FILE	*p;
	FILE	*q;
	int		id;
	int		i,	j;
	char	filename[100];

	int		itmp;
	BYTE	tmp;

	for( id = 1; id <= 36; id++ )
	{
		sprintf( filename, "../DATA/CNY100_Template/%02d.txt" , id );
		p	=	fopen( filename, "r" );

		sprintf( filename, "../DATA/CNY100_Template/%02d.bin", id );
		q	=	fopen( filename, "wb+" );

		for( j = 0; j < 32; j++ )
			for( i = 0; i < 22; i++ )
			{
				//fread( &tmp, sizeof(BYTE), 1, p );
				fscanf( p, "%d", &itmp );
				tmp		=	itmp;
				fwrite( &tmp, sizeof(BYTE), 1, q );
			}
		fclose(p);
		fclose(q);
	}*/
}
#endif