#include "stdafx.h"
#include "watershed.h"

#include <QImage>



static const POINT_W		FICTITIOUS(-1,-1);						// 虛擬點
static const POINT_W		W(-1,-1);								// alg 4.8 用來標示 Watershed用的. 不能屬於任何一個實際的點
//static const POINT		WW(-2,-2);
static const int		INIT	=	-1;							// minima detetor 
static const int		WSHED	=	0;							// mark 成 watershed. 不能跟其他的label起衝突
static const int		MASK	=	-2;
static const int		CON		=	4;							// 連通數.  4 或 8


/*=======================================================================================================
	struct point 的 == 操作
/*=======================================================================================================*/
bool operator == ( POINT_W a, POINT_W b )
{
		if( a.x == b.x && a.y == b.y )
			return	true;
		else
			return	false;
}

/*=======================================================================================================
	= operator
/*=======================================================================================================*/
POINT_W		POINT_W::operator	=	( POINT_W b )
{
	x		=	b.x;
	y		=	b.y;

	return		*this;
}

/*=======================================================================================================
	!= operator
/*=======================================================================================================*/
bool operator	!=	( POINT_W a, POINT_W b )
{
	return	!(a == b);
}




/*=================================================================================
	預設function
===================================================================================*/
Watershed::~Watershed()
{}


#ifdef _OPENPIV_

/*=======================================================================================================
	建構
	OPENPIV 底下使用
/*=======================================================================================================*/
Watershed::Watershed(Settings *settingsPass, DataContainer  *filedataPass)	
	: AndreaEngine()
{	
	progress	=	settingsPass->getProgress();	
	settings	=	settingsPass;
	filedata	=	filedataPass;

    mutex.lock();
    mutex.unlock();
}

#else

/*=======================================================================================================
	建構
	無傳入值
/*=======================================================================================================*/
Watershed::Watershed()	
{
	image.load("test.bmp");
	final.load("test.bmp");
}

#endif




/*=======================================================================================================
	NG 求有哪些鄰居
/*=======================================================================================================*/
void	Watershed::NG( POINT_W p, POINT_W *pointlist, int &size, int width, int height )
{
	int		count	=	0;

	if( p.y > 0 )
		pointlist[count++]	=	POINT_W( p.x, p.y-1 );

	if( p.y < height-1 )
		pointlist[count++]	=	POINT_W( p.x, p.y+1 ); 

	if( p.x > 0 )
		pointlist[count++]	=	POINT_W( p.x-1, p.y );

	if( p.x < width-1 )
		pointlist[count++]	=	POINT_W( p.x+1, p.y );

	size	=	count;
}




/*=======================================================================================================
	輸出
	檔名 = output.bmp
/*=======================================================================================================*/
void	Watershed::output()
{
	final.save("output.bmp");
}



/*=======================================================================================================
	lower completion
	轉換成lower complete的圖
/*=======================================================================================================*/
void	Watershed::LowerCompletion()
{
	boost::queue<POINT_W>		fifo_queue;

	QRgb			rgb1,	rgb2;
	int				gray1,	gray2;

	POINT_W			point,	q;
	POINT_W			pointlist[4];				// 用來處理NG 所有NG形成的set 假如要考慮8方向則需要修改


	lc.resize( image.width(), image.height() );

	int		i,	j;
	int		dist;
	int		size;

	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			lc(i,j)		=	0;

			if( has_low_neib( i, j ) )
			{
				lc( i, j )	=	-1;
				point		=	POINT_W( i ,j );
				fifo_queue.push(point);
			}
		}


	dist	=	1;

	fifo_queue.push( FICTITIOUS );			// 虛擬點

	while( !fifo_queue.empty() )
	{
		point	=	fifo_queue.top();		// remove拆成兩個動作
		fifo_queue.pop();

		if( point == FICTITIOUS )
		{
			if(  !fifo_queue.empty() )
			{
				fifo_queue.push( FICTITIOUS );
				dist++;
			}
		}
		else
		{
			lc( point.x, point.y )	=	dist;
			NG( point, pointlist, size, image.width(), image.height() );

			for( i=0; i<size; i++ )
			{
				q		=	pointlist[i];

				gray1	=	qGray( image.pixel(q.x, q.y) );
				gray2	=	qGray( image.pixel(point.x, point.y ) );

				//if( image.gray_channel( q.x, q.y ) == image.gray_channel( point.x, point.y ) && lc( q.x, q.y ) == 0 )
				if( gray1 == gray2 && lc( q.x, q.y ) == 0 )
				{
					fifo_queue.push( q );
					lc( q.x, q.y )	=	-1;
				}
			}
		}
	}



	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			gray1	=	qGray( image.pixel(i,j) );

			if( lc(i,j) != 0 )
				//lc(i,j)		=	dist * (int)image.gray_channel(i,j) + lc(i,j) - 1;// + max  ;
				lc(i,j)		=	dist * (int)gray1 + lc(i,j) - 1;// + max  ;
			else
				//lc(i,j)		=	dist * (int)image.gray_channel(i,j) ;//+ max;
				lc(i,j)		=	dist * (int)gray1 ;
		}


}


/*=======================================================================================================
	是否有比自己小的鄰居
/*=======================================================================================================*/
bool	Watershed::has_low_neib( int i, int j )
{
	QRgb	rgb1,	rgb2;
	int		gray1,	gray2;


	rgb1	=	image.pixel( i, j );
	gray1	=	qGray(rgb1);


	//int		self	=	image->gray_channel( i, j );
	int		self	=	gray1;

	POINT_W	up		=	POINT_W( i , j > 0 ? j-1 : j  );
	POINT_W	down	=	POINT_W( i , j < image.height()-1 ? j+1 : j   );
	POINT_W	left	=	POINT_W( i > 0 ? i-1 : i , j );
	POINT_W	right	=	POINT_W( i < image.width()-1 ? i+1 : i ,  j );

/*	if( image->gray_channel( up.x, up.y ) < self ||
		image->gray_channel( down.x, down.y ) < self ||
		image->gray_channel( left.x, left.y ) < self ||
		image->gray_channel( right.x, right.y ) < self  )*/
	if( qGray( image.pixel(up.x, up.y) ) < self ||
		qGray( image.pixel( down.x, down.y ) ) < self ||
		qGray( image.pixel( left.x, left.y ) ) < self ||
		qGray( image.pixel( right.x, right.y ) ) < self  )
			return	true;
	else
			return	false;

}


/*=======================================================================================================
	是否有比自己小的鄰居
	alg 4.1 使用版本
/*=======================================================================================================*/
bool	Watershed::has_low_neib_2( QImage *image, int i, int j )
{
	if( j>0 )
	{
		if( lab(i,j-1) > 0 || lab(i,j-1) == WSHED )
			return	true;
		else
			return	false;
	}

	if( j<image->height()-1 )
	{
		if( lab(i,j+1) > 0 || lab(i,j+1) == WSHED )
			return	true;
		else
			return	false;
	}
	if( i>0 )
	{
		if( lab(i-1,j) > 0 || lab(i-1,j) == WSHED )
			return	true;
		else
			return	false;
	}
	if( i<image->width()-1 )
	{
		if( lab(i+1,j) > 0 || lab(i+1,j) == WSHED )
			return	true;
		else
			return	false;
	}

	return	false;
}



/*=======================================================================================================
	sln   (實際上也可以掃一次存在陣列裡面)
/*=======================================================================================================*/
void	Watershed::sln_maker()
{
		// !!! 注意這邊的index i 是 1~4  而不是0~3
	int		width	=	lc.size1();
	int		height	=	lc.size2();
	int		min		=	INT_MAX;
	int		up,	down,	left,	right;
	int		i,	j;
	int		count;
	int		ii,	jj;


	POINT_W	p,	ptmp;
	POINT_W	plist[4];

	// !!! 注意這邊的index i 是 1~4  而不是0~3
	sln.resize( width*height, 5 );

	for( ii=0; ii<width; ii++ )
		for( jj=0; jj<height; jj++ )
		{
			min			=	INT_MAX;
			p			=	POINT_W(ii,jj);

			plist[0]	=	POINT_W(p.x,p.y-1);		// 上
			plist[1]	=	POINT_W(p.x,p.y+1);		// 下
			plist[2]	=	POINT_W(p.x-1,p.y);		// 左
			plist[3]	=	POINT_W(p.x+1,p.y);		// 右

			// sort plist by lc
			for( i=0; i<4; i++ )
				for( j=0; j<3-i; j++ )
				{
					// 點在範圍內
					if( plist[j].x >= 0 && plist[j].x < width && plist[j].y >= 0 && plist[j].y < height )
					{
						if( plist[j+1].x >= 0 && plist[j+1].x < width && plist[j+1].y >= 0 && plist[j+1].y < height )
						{
							if( lc( plist[j].x, plist[j].y ) > lc(plist[j+1].x, plist[j+1].y) )
							{
								ptmp		=	plist[j];
								plist[j]	=	plist[j+1];
								plist[j+1]	=	ptmp;
							}
						}
						//else{}   兩個都能比,才做比較. 不然就不必交換了.
					}
					// 點在範圍外 都視為最大點
					else
					{
						ptmp		=	plist[j];
						plist[j]	=	plist[j+1];
						plist[j+1]	=	ptmp;	
					}

				}

			for( i=0; i<4; i++ )
			{
				if( plist[i].x >= 0 && plist[i].x < width && plist[i].y >= 0 && plist[i].y < height )
				{
					// !!! 注意這邊的index i 是 1~4  而不是0~3
					if( lc(p.x,p.y) > lc( plist[i].x,plist[i].y )  )
						sln( p.x*height + p.y, i+1 )	=	plist[i];
					else if( i == 0 )
						sln( p.x*height + p.y, 1 )		=	p;
					else
						sln( p.x*height + p.y, i+1 )	=	p;
				}
				else
					sln( p.x*height + p.y, i+1 )	=	p;
			}


#if 0
			up		=	p.y > 0             ?   lc( p.x, p.y-1 )        :   INT_MAX;
			down	=	p.y < height - 1    ?   lc( p.x, p.y+1 )	    :   INT_MAX;
			left	=	p.x > 0             ?   lc( p.x-1, p.y )        :   INT_MAX;
			right	=	p.x < width - 1     ?   lc( p.x+1, p.y )		:   INT_MAX;

			// 搜尋最小值
			if( up < min )		min		=	up;
			if( down < min )	min		=	down;
			if( left < min )	min		=	left;
			if( right < min )	min		=	right;

			// 將資料存放到 sln裡面
			count	=	1;
			// 避免大家都是 INIT_MAX的狀況. 以及流到相對低點的情況
			/*if( min < INT_MAX && min < lc(p.x, p.y) )
			{
				if( up == min )		sln( p.x*height + p.y , count++ )	=	POINT(p.x, p.y-1 );
				if( down == min )	sln( p.x*height + p.y , count++ )	=	POINT(p.x, p.y+1 );
				if( left == min )	sln( p.x*height + p.y , count++ )	=	POINT(p.x-1, p.y );
				if( right == min )	sln( p.x*height + p.y , count++ )	=	POINT(p.x+1, p.y );
			}*/

			if( min < INT_MAX /*&& min < lc(p.x, p.y)*/ )
			{
				if( up < lc(p.x, p.y) )		sln( p.x*height + p.y , count++ )	=	POINT(p.x, p.y-1 );
				if( down < lc(p.x, p.y) )	sln( p.x*height + p.y , count++ )	=	POINT(p.x, p.y+1 );
				if( left < lc(p.x, p.y) )	sln( p.x*height + p.y , count++ )	=	POINT(p.x-1, p.y );
				if( right < lc(p.x, p.y) )	sln( p.x*height + p.y , count++ )	=	POINT(p.x+1, p.y );
			}

			// 補足剩下的值.  (指向自己  表示自己就是自己的parent, 自己就是 root)
			for( ; count<=4; count++ )
				sln( p.x*height + p.y , count )	=	p;
#endif
		}


	// !!! 注意這邊的index i 是 1~4  而不是0~3
}



/*=======================================================================================================
	遞迴使用的 Resolve
/*=======================================================================================================*/
POINT_W	Watershed::Resolve( POINT_W p )
{
	int			i		=	1;	
	int			j;
	const int	index	=	p.x * lc.size2() + p.y;
	POINT_W		rep(0,0),	ptmp;

	while( i<=CON && rep != W )
	{
		ptmp	=	sln(index,i);

		/*if( ptmp == p )
		{
			if( i == 1 )
			{
				rep	=	sln(index,1);
				break;
			}
		}*/

		if( sln(index,i) != p && sln(index,i) != W )
			sln(index,i)	=	Resolve( sln(index,i) );

		POINT_W	ptmp2	=	sln(index,i);

		if( i == 1 )
			rep		=	sln( index, 1 );
		
		else if( ptmp2 == W )
		{
			rep		=	W;
			for( j=1; j<=CON; j++ )
				sln(index,j)	=	W;
		}

		else if( ptmp2 == p )
		{
			if( i == 1 )
			{
				rep		=	p;
				break;
			}
			/*else if( lab( ptmp2.x, ptmp2.y ) != lab( rep.x, rep.y ) )
			{
				rep		=	W;
				for( j=1; j<=CON; j++ )
					sln(index,j)	=	W;
			}*/
		}

		//else if( sln(index,i) != rep )
		else if( lab( ptmp2.x, ptmp2.y ) != lab( rep.x, rep.y ) )
		{
			rep		=	W;
			for( j=1; j<=CON; j++ )
				sln(index,j)	=	W;
		}

		i++;
	}

	return	rep;
}





/*=======================================================================================================
	執行watershed
	論文的alg4.8  
	用遞迴的方式
/*=======================================================================================================*/
void	Watershed::doWatershed()
{
	int			width	=	lc.size1();
	int			height	=	lc.size2();
	int			i,	j;
	POINT_W		rep;
	POINT_W		p;


	// 建立sln table
	sln_maker();

	for( i=0; i<width; i++ )
		for( j=0; j<height; j++ )
		{
			//if( i == 260 && j == 290 )
			//	std::cout << "doing " << i << " " << j << "\n";

			p		=	POINT_W(i,j);
			rep		=	Resolve( p );
			if( rep != W )
				lab( p.x, p.y )		=	lab( rep.x, rep.y );
			else
				lab( p.x, p.y )		=	WSHED;
		}

	//std::cout << "finished...\n";

	// 將資料回寫到圖片
	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			if( lab(i,j) == WSHED )
				//image.pixel_channel( i, j, 255, 0, 0 );
				final.setPixel( i, j, qRgb(255,0,0) );			// 將watershed畫成紅色
				//	image.gray_channel( i, j, lab(i,j)*10 );
			//else
			//	final.gray_channel( i, j, lab(i,j)*10 );		// 其他的依照不同lab來畫出不同顏色

			// 跟隔壁不同lab的也畫上顏色 ( 避免線斷掉)
			if( i>=1 && i<image.width()-1 && j>=1 && j<image.height()-1 )
			{
				if( lab( i,j) != lab(i+1,j) || lab(i,j) != lab(i-1,j) || lab(i,j) != lab(i,j+1) || lab(i,j) != lab(i,j-1 ) )
					final.pixel_channel( i, j, 255, 0, 0 );
			}
		}

}


/*=======================================================================================================
	minima detetor
	找出最小值  並且標示之
	(應該是用lower complete的圖)
/*=======================================================================================================*/
void	Watershed::LevelComponents()
{
	lab.resize( image.width(), image.height() );

	boost::queue<POINT_W>			fifo_queue;

	POINT_W		p,	q,	s,	pointlist[4];

	QRgb	rgb1,	rgb2;
	int		gray1,	gray2;


	int				i,	j,	k;
	int				size;
	unsigned int	curlab;			// current label

	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
			lab(i,j)	=	INIT;

	curlab		=	1;

	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			if( lab(i,j) == INIT )
			{
				lab(i,j)	=	curlab;
				p			=	POINT_W(i,j);
				fifo_queue.push( p );

				while( !fifo_queue.empty() )
				{
					s	=	fifo_queue.top();
					fifo_queue.pop();

					NG( s, pointlist, size, image.width(), image.height() );

					for( k=0; k<size; k++ )
					{
						q	=	pointlist[k];

						gray1	=	qGray( image.pixel(s.x,s.y) );
						gray2	=	qGray( image.pixel( q.x, q.y ) );

						//if( lc( s.x, s.y ) == lc( q.x, q.y ) )
						//if( image.gray_channel(s.x, s.y) == image.gray_channel( q.x, q.y ) )
						//if( abs( image.gray_channel(s.x, s.y) - image.gray_channel( q.x, q.y ) ) < 4 )
						if( gray1 == gray2 )
						//if( abs( gray1 - gray2 ) < 5 )
						{
							if( lab( q.x, q.y ) == INIT )
							{
								lab( q.x, q.y )	=	curlab;
								fifo_queue.push( q );
							}
						}
					}

				}
				curlab++;
			}
		}


	/*QImage	ILabel( image.width(), image.height(), QImage::Format_RGB888 );
	for( i = 0; i < ILabel.width(); i++ )
		for( j = 0; j < ILabel.height(); j++ )
			ILabel.setPixel( i, j, lab(i,j) );
	ILabel.save("label.bmp");*/

}



/*=======================================================================================================
	最原始的watershed   alg 4.1
/*=======================================================================================================*/
void	Watershed::doWatershed_4_1()
{
	int		curlab	=	0;
	int		i,	j;
	int		ii,	jj;
	
	boost::queue<POINT_W>		fifo_queue;

	int		width	=	image.width();
	int		height	=	image.height();

	lab.resize( width, height );
	dist.resize( width, height );

	for( i=0; i<width; i++ )
		for( j=0; j<height; j++ )
		{
			lab(i,j)	=	INIT;
			dist(i,j)	=	0;
		}

	// sort by gray value
	typedef boost::heap::priority_queue<int>	PriorityQueue;
	
	PriorityQueue	pq;

	//PriorityQueue::iterator		begin	=	sort_data.begin();
	//PriorityQueue::iterator		end		=	sort_data.end();
	//boost::heap::priority_queue<int>::iterator	itr;
	bool	is;
	for( i=0; i<width; i++ )
		for( j=0; j<height; j++ )
		{
			is	=	true;
			for( PriorityQueue::iterator itr = pq.begin();  itr!=pq.end();  itr++  )
			{
				//if( *itr == image.gray_channel(i,j) )
				if( *itr == qGray( image.pixel(i,j) ) )
				{
					is	=	false;
					break;
				}
			}

			if( is )
				pq.push(  qGray( image.pixel(i,j) ) );

		}

	sort_data.resize( pq.size() );

	//while( !pq.empty() )
	for( i=sort_data.size()-1; i>=0; i-- )
	{
		//std::cout << pq.top() << " ";
		sort_data(i)	=	pq.top();
		pq.pop();
	}
	// output
	for( i=0; i<sort_data.size(); i++ )
		std::cout << sort_data(i) << " ";
	std::cout << std::endl;
	




	int			curdist,	size;
	int			h;
	POINT_W		p,	q,	r;
	POINT_W		pointlist[4];

	for( ii=0; ii<sort_data.size(); ii++ )
	{

		h	=	sort_data(ii);
		std::cout << h << "... \n";

		for( i=0; i<width; i++ )
			for( j=0; j<height; j++ )
			{
				if(  qGray( image.pixel(i,j) ) == sort_data(ii) )
				{
					lab(i,j)	=	MASK;
					if( has_low_neib_2( &image, i,j ) )
					{
						dist(i,j)	=	1;
						fifo_queue.push( POINT_W(i,j) );
					}
				}
			}
		curdist	=	1;
		fifo_queue.push( FICTITIOUS);
		

		while(1)
		{
			p	=	fifo_queue.top();
			fifo_queue.pop();

			if( p == FICTITIOUS )
			{
				if( fifo_queue.empty() )
					break;
				else
				{
					fifo_queue.push( FICTITIOUS );
					curdist++;
					p	=	fifo_queue.top();
					fifo_queue.pop();
				}

			}


			NG( p, pointlist, size, width,  height );

			for( i=0; i<size; i++ )
			{
				q	=	pointlist[i];
				if( dist( q.x, q.y ) < curdist && ( lab(q.x,q.y)>0 || lab(q.x,q.y) == WSHED ) )
				{
					if( lab(q.x,q.y)>0 )
					{
						if( lab(p.x,p.y) == MASK || lab(p.x,p.y) == WSHED )
							lab(p.x,p.y)	=	lab(q.x,q.y);
						else if( lab(p.x,p.y) != lab(q.x,q.y) )
							lab(p.x,p.y)	=	WSHED;
					}
					else if( lab(p.x,p.y) == MASK )
						lab(p.x,p.y)	=	WSHED;
				}
				else if( lab(q.x,q.y) == MASK && dist(q.x,q.y) == 0 )
				{
					dist(q.x,q.y)	=	curdist + 1;
					fifo_queue.push(q);
				}
			}		
		}		// end whlie


	
		for( i=0; i<width; i++ )
		{
			for( j=0; j<height; j++ )
			{
				if( qGray( image.pixel(i,j) ) == h )
				{
					p	=	POINT_W(i,j);
					dist(p.x,p.y)	=	0;
					if( lab(p.x,p.y) == MASK )
					{
						curlab	=	curlab + 1;
						fifo_queue.push(p);
						lab(p.x,p.y)	=	curlab;

						while( !fifo_queue.empty() )
						{
							q	=	fifo_queue.top();
							fifo_queue.pop();

							NG( q, pointlist, size, width, height );
							for( jj=0; jj<size; jj++ )
							{
								r	=	pointlist[jj];
								if( lab(r.x,r.y) == MASK )
								{
									fifo_queue.push(r);
									lab(r.x,r.y)	=	curlab;
								}
							}
						}

					}
				}
			}
		}

	}	// end for( ii=0; ii<sort_data.size(); ii++ )




	for( i=0; i<width; i++ )
		for( j=0; j<height; j++ )
		{
			if( lab(i,j) == WSHED )
				final.setPixel( i, j, qRgb(255,0,0) );
				//image.pixel_channel(i,j,255,0,0);
			else
				final.setPixel( i, j,lab(i,j)*10);
			//	image.gray_channel( i, j, lab(i,j)*10 );

			// 將不同區塊間都畫線 (簡易將線連起來)
			if( i>=1 && i<width-1 && j>=1 && j<height-1 )
			{
			//	if( lab(i,j) != lab(i+1,j) || lab(i,j) != lab(i-1,j) || lab(i,j) != lab(i,j+1) || lab(i,j) != lab(i,j-1) )
			//		final.pixel_channel( i, j, 255, 0, 0 );
			}

		}


}







/*=======================================================================================================
	反白
/*=======================================================================================================*/
void	Watershed::highlight()
{
	int		gray;
	int		i,	j;

	for( i=0; i<image.width(); i++ )
	{
		for( j=0; j<image.height(); j++ )
		{
			gray	=	qGray( image.pixel(i,j) );
			gray	=	255 - gray;
			image.setPixel( i, j, qRgb(gray,gray,gray) );
		}
	}
}


/*=======================================================================================================
	將圖形做truncate   
	i+-5 都變成 i 之類的
	要取幾個區間則需要設計
/*=======================================================================================================*/
QImage	Watershed::truncate( int n )
{
	QImage	img( image.width(), image.height(), QImage::Format_RGB888 );
	img.fill(0);

	const int	N	=	256/n;			// 間隔數 
	int			i,	j;
	int			mag;

	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			mag		=	image.gray_channel( i, j );
			mag		=	(mag/N) * N;
			img.gray_channel( i, j, mag );
		}

	img.save("truncate.bmp");
	return	img;
}




/*=======================================================================================================
	產生gradient image    sqrt( fx^2 + fy^2 )
/*=======================================================================================================*/
QImage	Watershed::gradient()
{
	int		i,	j;
	double	fx,	fy;
	double	mag;

	QImage	img( image.width(), image.height(), QImage::Format_RGB888 );
	img.fill(0);	// 歸零

	for( i=1; i<image.width()-1; i++ )
		for( j=1; j<image.height()-1; j++ )
		{
			fx	=	0.5 * ( image.gray_channel(i+1,j) - image.gray_channel(i-1,j) );
			fy	=	0.5 * ( image.gray_channel(i,j+1) - image.gray_channel(i,j-1) );
			mag	=	sqrt( fx*fx + fy*fy );

			img.gray_channel( i, j, mag );
		}
	img.save("gradient.bmp");
	return	img;
}

/*=======================================================================================================
	將圖形切開來  
	色標用
/*=======================================================================================================*/
QImage	Watershed::divide( QImage	imagePass )
{
	int		i,	j;

	GaussFilter_2D	gf(1,1);		// 做filter

	//Watershed	watershed;
	image	=	imagePass;
	final	=	image;
	
	image	=	gf.do_filter( image, progress );
	image	=	gradient();			// 將圖形做gradient  找出邊

	// 底下是原本的4.8  前面是預處理
	LowerCompletion();
	LevelComponents();
	doWatershed();


	// 產生label資訊  (讓色標檢測使用)
	for( i=0; i<image.width(); i++ )
		for( j=0; j<image.height(); j++ )
		{
			if( lab(i,j) == WSHED )
				final.pixel_channel( i, j, 255, 255, 255 );				// 假如currenlab跑到白色  那就一定會發生錯誤
			else
				final.setPixel( i, j, lab(i,j) );
				//final.pixel_channel( i, j, lab(i,j) , 0, 0 );
		}
	//final.save("watersh48.bmp");

	return	final;
}



#ifdef _OPENPIV_
/*=======================================================================================================
	利用這邊取代main  執行各種動作 
/*=======================================================================================================*/
void	Watershed::dosomething()
{
	GaussFilter_2D	gf(1,1);		// 做filter

	//Watershed	watershed;
	image	=	filedata->get_image();
	final	=	image;

	// 是否要反白
	if( settings->isHighlight )
		highlight();

	// 做gradient
	//gradient();
	// 這邊的演算法是  先將圖形做成gradient map, 再利用gradient去找watershed. 用於4.8 不適用MSER
	//image	=	gradient();
	

	switch( settings->watershed )
	{
		case OpenPIV::i4_8 :

			image	=	gf.do_filter( image, progress );
			//image	=	truncate(8);		// 將圖形做truncate, 避免過多的雜訊   傳入值表示希望切割成幾個level
			image	=	gradient();			// 將圖形做gradient  找出邊

			// 底下是原本的4.8  前面是預處理
			LowerCompletion();
			LevelComponents();
			doWatershed();
			break;

		case OpenPIV::i4_1 :

			image	=	gf.do_filter( image, progress );
			//image	=	truncate(8);		// 將圖形做truncate, 避免過多的雜訊   傳入值表示希望切割成幾個level
			image	=	gradient();			// 將圖形做gradient  找出邊

			// 底下是原本4.1
			doWatershed_4_1();
			break;

		case OpenPIV::MSER :
			final	=	mser_process(image);
			break;

		default:
			break;
	}

	//watershed.doWatershed_4_1();
	//	mser_process();
	//emit( filedata->output(final) );
	emit( progress->outputImage( 0, final ) );
	progress->debuger("<font color=\"Red\"> finish watershed blob detector... </font>");

}

#endif