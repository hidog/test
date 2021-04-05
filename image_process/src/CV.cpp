#include "stdafx.h"
#include "CV.h"
/////////////////////////////// AutoBuffer ////////////////////////////////////////


template<typename _Tp> static inline _Tp* allocate_1(size_t n)
{
    return new _Tp[n];
}

template<typename _Tp> static inline void deallocate_1(_Tp* ptr, size_t)
{
    delete[] ptr;
}


template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>::AutoBuffer()
{
    ptr = buf;
    size = fixed_size;
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>::AutoBuffer(size_t _size)
{
    ptr = buf;
    size = fixed_size;
    allocate(_size);
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>::~AutoBuffer()
{ deallocate(); }

template<typename _Tp, size_t fixed_size> inline void AutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
    if(_size <= size)
        return;
    deallocate();
    if(_size > fixed_size)
    {
        ptr = allocate_1<_Tp>(_size);
        size = _size;
    }
}

template<typename _Tp, size_t fixed_size> inline void AutoBuffer<_Tp, fixed_size>::deallocate()
{
    if( ptr != buf )
    {
        deallocate_1<_Tp>(ptr, size);
        ptr = buf;
        size = fixed_size;
    }
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>::operator _Tp* ()
{ return ptr; }

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>::operator const _Tp* () const
{ return ptr; }




typedef union Cv32suf
{
    int i;
    unsigned u;
    float f;
}
Cv32suf;

CV_INLINE  int  cvRound( double value )
{

    int t;
    __asm
    {
        fld value;
        fistp t;
    }
    return t;

}


int  cvFloor( double value )
{
    int i = cvRound(value);
    Cv32suf diff;
    diff.f = (float)(value - i);
    return i - (diff.i < 0);
}





////////////////////////////////////////////////////////////////////////////////////////////////////////////




void icvSepConvSmall3_32f( float* src, int src_step, float* dst, int dst_step,
            CvSize src_size, const float* kx, const float* ky, float* buffer )
{
    int  dst_width, buffer_step = 0;
    int  x, y;

    assert( src && dst && src_size.width > 2 && src_size.height > 2 &&
            (src_step & 3) == 0 && (dst_step & 3) == 0 &&
            (kx || ky) && (buffer || !kx || !ky));

    src_step /= sizeof(src[0]);
    dst_step /= sizeof(dst[0]);

    dst_width = src_size.width - 2;

    if( !kx )
    {
        /* set vars, so that vertical convolution
           will write results into destination ROI and
           horizontal convolution won't run */
        src_size.width = dst_width;
        buffer_step = dst_step;
        buffer = dst;
        dst_width = 0;
    }

    assert( src_step >= src_size.width && dst_step >= dst_width );

    src_size.height -= 3;
    if( !ky )
    {
        /* set vars, so that vertical convolution won't run and
           horizontal convolution will write results into destination ROI */
        src_size.height += 3;
        buffer_step = src_step;
        buffer = src;
        src_size.width = 0;
    }

    for( y = 0; y <= src_size.height; y++, src += src_step,
                                           dst += dst_step,
                                           buffer += buffer_step )
    {
        float* src2 = src + src_step;
        float* src3 = src + src_step*2;
        for( x = 0; x < src_size.width; x++ )
        {
            buffer[x] = (float)(ky[0]*src[x] + ky[1]*src2[x] + ky[2]*src3[x]);
			//std::cout << buffer[x] << " ";
        }
		//std::cout << "\n\n";

        for( x = 0; x < dst_width; x++ )
        {
            dst[x] = (float)(kx[0]*buffer[x] + kx[1]*buffer[x+1] + kx[2]*buffer[x+2]);
			//std::cout << dst[x] << " ";
        }
		//std::cout << "\n";
    }
}


void  icvGetRectSubPix_8u32f_C1R
( const uchar* src, int src_step, CvSize src_size,
  float* dst, int dst_step, CvSize win_size, CvPoint2D32f center )
{
    CvPoint ip;
    float  a12, a22, b1, b2;
    float a, b;
    double s = 0;
    int i, j;

    center.x -= (win_size.width-1)*0.5f;
    center.y -= (win_size.height-1)*0.5f;

    ip.x = cvFloor( center.x );
    ip.y = cvFloor( center.y );

   // if( win_size.width <= 0 || win_size.height <= 0 )
   //     return CV_BADRANGE_ERR;

    a = center.x - ip.x;
    b = center.y - ip.y;
    a = MAX(a,0.0001f);
    a12 = a*(1.f-b);
    a22 = a*b;
    b1 = 1.f - b;
    b2 = b;
    s = (1. - a)/a;

    src_step /= sizeof(src[0]);
    dst_step /= sizeof(dst[0]);

    if( 0 <= ip.x && ip.x + win_size.width < src_size.width &&
        0 <= ip.y && ip.y + win_size.height < src_size.height )
    {
        // extracted rectangle is totally inside the image
        src += ip.y * src_step + ip.x;

#if 0
        if( icvCopySubpix_8u32f_C1R_p &&
            icvCopySubpix_8u32f_C1R_p( src, src_step, dst,
                dst_step*sizeof(dst[0]), win_size, a, b ) >= 0 )
            return CV_OK;
#endif

        for( ; win_size.height--; src += src_step, dst += dst_step )
        {
            float prev = (1 - a)*(b1*CV_8TO32F(src[0]) + b2*CV_8TO32F(src[src_step]));
            for( j = 0; j < win_size.width; j++ )
            {
                float t = a12*CV_8TO32F(src[j+1]) + a22*CV_8TO32F(src[j+1+src_step]);
                dst[j] = prev + t;
                prev = (float)(t*s);
				//std::cout << dst[j] << " ";
            }
			//std::cout << "\n";
        }
    }
    else
    {
		std::cout << "!!!!!";

       /* CvRect r;

        src = (const uchar*)icvAdjustRect( src, src_step*sizeof(*src),
                               sizeof(*src), src_size, win_size,ip, &r);

        for( i = 0; i < win_size.height; i++, dst += dst_step )
        {
            const uchar *src2 = src + src_step;

            if( i < r.y || i >= r.height )
                src2 -= src_step;

            for( j = 0; j < r.x; j++ )
            {
                float s0 = CV_8TO32F(src[r.x])*b1 +
                           CV_8TO32F(src2[r.x])*b2;

                dst[j] = (float)(s0);
            }

            if( j < r.width )
            {
                float prev = (1 - a)*(b1*CV_8TO32F(src[j]) + b2*CV_8TO32F(src2[j]));

                for( ; j < r.width; j++ )
                {
                    float t = a12*CV_8TO32F(src[j+1]) + a22*CV_8TO32F(src2[j+1]);
                    dst[j] = prev + t;
                    prev = (float)(t*s);
                }
            }

            for( ; j < win_size.width; j++ )
            {
                float s0 = CV_8TO32F(src[r.width])*b1 +
                           CV_8TO32F(src2[r.width])*b2;

                dst[j] = (float)(s0);
            }

            if( i < r.height )
                src = src2;
        }*/
    }

}

CvMat* cvGetMat( QImage image, CvMat *stub )
{

	int		i,	j;

	stub->data.ptr	=	new uchar[ image.width() * image.height() ];
	stub->step		=	image.width();

	for( j=0; j<image.height(); j++ )
		for( i=0; i<image.width(); i++ )
		{
			stub->data.ptr[ i + j*stub->step ]	=	image.gray_channel( i, j );
		}


	return	stub;
}



void cvFindCornerSubPix2( QImage srcarr, CvPoint2D32f* corners,  int count, CvSize win, CvSize zeroZone )
{
	AutoBuffer<float> buffer;
	//float	*buffer;
    
    const int MAX_ITERS = 100;
    const float drv_x[] = { -1.f, 0.f, 1.f };
    const float drv_y[] = { 0.f, 0.5f, 0.f };
    float *maskX;
    float *maskY;
    float *mask;
    float *src_buffer;
    float *gx_buffer;
    float *gy_buffer;
    int win_w = win.width * 2 + 1, win_h = win.height * 2 + 1;
    int win_rect_size = (win_w + 4) * (win_h + 4);
    double coeff;
    CvSize size, src_buf_size;
    int i, j, k, pt_i;
    int max_iters = 1000;
    double eps = 0;

    CvMat stub, *src;
    src = cvGetMat( srcarr, &stub );

	size.width  = srcarr.width();
	size.height = srcarr.height();

    /* initialize variables, controlling loop termination */


    eps = MAX( eps, 0 );
    eps *= eps;                 /* use square of error in comparsion operations. */
	eps	=	0.001;

    max_iters = MAX( max_iters, 1 );
    max_iters = MIN( max_iters, MAX_ITERS );

    buffer.allocate( win_rect_size * 5 + win_w + win_h + 32 );
	//buffer	=	new float[ win_rect_size*5 + win_w + win_h + 32 ];

    /* assign pointers */
    maskX = buffer;
    maskY = maskX + win_w + 4;
    mask = maskY + win_h + 4;
    src_buffer = mask + win_w * win_h;
    gx_buffer = src_buffer + win_rect_size;
    gy_buffer = gx_buffer + win_rect_size;

	/*std::cout << (int)(maskX - buffer) << " "
			<< (int)(maskY - buffer) << " "
			<< (int)(mask - buffer) << " "
			<< (int)(src_buffer - buffer) << " "
			<< (int)(gx_buffer - buffer) << " "
			<< (int)(gy_buffer - buffer) << "\n" ;*/

    coeff = 1. / (win.width * win.width);

    /* calculate mask */
    for( i = -win.width, k = 0; i <= win.width; i++, k++ )
    {
        maskX[k] = (float)exp( -i * i * coeff );
    }

    if( win.width == win.height )
    {
        maskY = maskX;
    }
    else
    {
        coeff = 1. / (win.height * win.height);
        for( i = -win.height, k = 0; i <= win.height; i++, k++ )
        {
            maskY[k] = (float) exp( -i * i * coeff );
        }
    }

    for( i = 0; i < win_h; i++ )
    {
        for( j = 0; j < win_w; j++ )
        {
            mask[i * win_w + j] = maskX[j] * maskY[i];
			//std::cout << mask[i*win_w+j] << " ";
        }
		//std::cout << "\n";
    }



    /* make zero_zone */
    if( zeroZone.width >= 0 && zeroZone.height >= 0 &&
        zeroZone.width * 2 + 1 < win_w && zeroZone.height * 2 + 1 < win_h )
    {
        for( i = win.height - zeroZone.height; i <= win.height + zeroZone.height; i++ )
        {
            for( j = win.width - zeroZone.width; j <= win.width + zeroZone.width; j++ )
            {
                mask[i * win_w + j] = 0;
            }
        }
    }

    /* set sizes of image rectangles, used in convolutions */
    src_buf_size.width = win_w + 2;
    src_buf_size.height = win_h + 2;

    /* do optimization loop for all the points */
    for( pt_i = 0; pt_i < count; pt_i++ )
    {
        CvPoint2D32f cT = corners[pt_i], cI = cT;
        int iter = 0;
        double err;

        do
        {
            CvPoint2D32f cI2;
            double a, b, c, bb1, bb2;

            icvGetRectSubPix_8u32f_C1R( (uchar*)src->data.ptr, src->step, size,
                                        src_buffer, (win_w + 2) * sizeof( src_buffer[0] ),
                                        cvSize( win_w + 2, win_h + 2 ), cI );

            /* calc derivatives */
            icvSepConvSmall3_32f( src_buffer, src_buf_size.width * sizeof(src_buffer[0]),
                                  gx_buffer, win_w * sizeof(gx_buffer[0]),
                                  src_buf_size, drv_x, drv_y, buffer );

            icvSepConvSmall3_32f( src_buffer, src_buf_size.width * sizeof(src_buffer[0]),
                                  gy_buffer, win_w * sizeof(gy_buffer[0]),
                                  src_buf_size, drv_y, drv_x, buffer );

            a = b = c = bb1 = bb2 = 0;

            /* process gradient */
            for( i = 0, k = 0; i < win_h; i++ )
            {
                double py = i - win.height;

                for( j = 0; j < win_w; j++, k++ )
                {
                    double m = mask[k];
					//std::cout << m << " ";
                    double tgx = gx_buffer[k];
                    double tgy = gy_buffer[k];
                    double gxx = tgx * tgx * m;
                    double gxy = tgx * tgy * m;
                    double gyy = tgy * tgy * m;
                    double px = j - win.width;

                    a += gxx;
                    b += gxy;
                    c += gyy;

                    bb1 += gxx * px + gxy * py;
                    bb2 += gxy * px + gyy * py;
                }
				//std::cout << "\n";
            }

            {
                double A[4];
                double InvA[4];
                //CvMat matA, matInvA;

                A[0] = a;
                A[1] = A[2] = b;
                A[3] = c;

               // cvInitMatHeader( &matA, 2, 2, CV_64F, A );
               // cvInitMatHeader( &matInvA, 2, 2, CV_64FC1, InvA );
               // cvInvert( &matA, &matInvA, CV_SVD );

				double	det	=	(A[0]*A[3] - A[1]*A[2]);
				if( det == 0 )	det	+=	0.000000000000000001;

				InvA[0]	=	A[3] / det;
				InvA[1]	=	-A[1] / det;
				InvA[2]	=	-A[2] / det;
				InvA[3]	=	A[0] / det;

                cI2.x = (float)(cI.x + InvA[0]*bb1 + InvA[1]*bb2);
                cI2.y = (float)(cI.y + InvA[2]*bb1 + InvA[3]*bb2);
            }

            err = (cI2.x - cI.x) * (cI2.x - cI.x) + (cI2.y - cI.y) * (cI2.y - cI.y);
            cI = cI2;
        }
        while( ++iter < max_iters && err > eps );

        /* if new point is too far from initial, it means poor convergence.
           leave initial point as the result */
        if( fabs( cI.x - cT.x ) > win.width || fabs( cI.y - cT.y ) > win.height )
        {
            cI = cT;
			std::cout << "!!!error";
        }

        corners[pt_i] = cI;     /* store result */
    }
}



void cornerSubPix( QImage image, double cx, double cy, double &dx, double &dy )
{
	image.save("test_output.bmp");

	CvPoint2D32f	corners[10];
	corners[0].x	=	cx;
	corners[0].y	=	cy;
	
	int		count	=	1;
	
	CvSize	win;
	win.width	=	11;
	win.height	=	11;

	CvSize	zeroZone;
	zeroZone.width	=	-1;
	zeroZone.height	=	-1;
	

	cvFindCornerSubPix2( image, corners,  count, win,  zeroZone );

	dx	=	corners[0].x;
	dy	=	corners[0].y;

}