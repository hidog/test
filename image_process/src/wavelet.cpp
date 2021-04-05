/* ******************************************************************
                 WAVELET TRANSFORM PROGRAM

The subprogram called "wavelet_2d(image,m,n,l,s)" (or 
"i_wavelet_2d(image,m,n,s)")

is to perform the forward (or inverse) 2-D wavelet transform.

"image" -- 2-D double array representing
the input data and will be replaced by the output data.

"m","n" -- size of the image used.

"level" -- level required to perform the wavelet transform.

"s" -- kernels used for transformation.

Please note that this program can perform different types of kernels,
which are
   0 : for Spline
   1 : for Haar basis
   2 : for Daubechies 4-tap compactly supported wavelet
   4 : for Daubechies 8-tap compactly supported wavelet
   8 : for Daubechies 16-tap compactly supported wavelet.

********************************************************************** */

double spline[21]
  = {  0.7661300537597422,  0.4339226335893024, -0.0502017246714322,
      -0.1100370183880987,  0.0320808974701767,  0.0420683514407039,
      -0.0171763154919797, -0.0179823209809603,  0.0086852948130698,
       0.0082014772059938, -0.0043538394577629, -0.0038824252655926,
       0.0021867123701413,  0.0018821335238871, -0.0011037398203844,
      -0.0009271987314557,  0.0005599366433624,  0.0004621152275223,
      -0.0002853837186721, -0.0002323472940263,  0.0001460418697826 };
double d4[4]
  = { 0.482962913145, 0.836516303738, 0.224143868042, -0.129409522551 };
double d8[8]
  = { 0.230377813309, 0.714846570553, 0.630880767930, -0.027983769417,
      -0.187034811719, 0.030841381836, 0.032883011667, -0.010597401785 };
double d16[16]
  = { 0.054415842243,  0.312871590914,  0.675630736297,  0.585354683654,
     -0.015829105256, -0.284015542962,  0.000472484574,  0.128747426620,
     -0.017369301002, -0.044088253931,  0.013981027917,  0.008746094047,
     -0.004870352993, -0.000391740373,  0.000675449409, -0.000117476784 };

/* ------------------------------------------------------------ */

wavelet_2d(image, m, n, l, s)
double *image;
int m, n, l, s;
{
  int i, j, ii;
  double tmp;

  if (l == 0)         /* check termination condition */
    {
      return(n);
    }

  switch (s) {
  case 0:
    if (n == 8)
      return(n);
    break;
  case 1:
    if (n == 1)
      return(n);
    break;
  case 2:
    if (n == 2)
      return(n);
    break;
  case 4:
    if (n == 4)
      return(n);
    break;
  case 8:
    if (n == 8)
      return(n);
    break;
  default:          /* undefined scaling function */
    return(-1);
  }

  for (i = 0; i < n; i++)    /* rowwise decomposition */
    decomposition(image+i*m, image+i*m, image+i*m+n/2, n, s);

  for (i = 1; i < n; i++) {   /* matrix transpose */
    ii = i*m;
    for (j = 0; j < i; j++) {
      tmp = image[ii+j];
      image[ii+j] = image[j*m+i];
      image[j*m+i] = tmp;
    }
  }
  for ( i = 0; i < n; i++)   /* columnwise decomposition */
    decomposition(image+i*m, image+i*m, image+i*m+n/2, n, s);

  for (i = 1; i < n; i++) {   /* another matrix transpose */
    ii = i*m;
    for (j = 0; j < i; j++) {
      tmp = image[ii+j];
      image[ii+j] = image[j*m+i];
      image[j*m+i] = tmp;
    }
  }
  return(wavelet_2d(image, m/2, n/2, --l, s));    /* recursion */
}
/* --------------------------------------------------------------- */
i_wavelet_2d(image, m, n, s)
double *image;
int m, n, s;
{
  int i, j, ii, jj, k, kk1, kk2, nn, th1;
  double tmp;

  if (m == n)      /* check termination condition */
    return;

  nn = n << 1;
  th1 = m/nn;

  if (nn != m) {
   for (k=0; k<th1; k++) {
    for (j=0; j<th1; j++) {
     kk1 = nn*(m*k +j);
     for (i = 0; i < nn; i++)   /* rowwise reconstruction */
      reconstruction(image+kk1+i*m, image+kk1+i*m, image+kk1+i*m+n, n, s);

     for (i = 1; i < nn; i++) {   /* matrix transpose */
      kk2 = i*m + kk1;
      for (jj = 0; jj < i; jj++) {
       tmp = image[kk2+jj];
       image[kk2+jj] = image[jj*m+kk1+i];
       image[jj*m+kk1+i] = tmp;
      }
     }

     for (i = 0; i < nn; i++)   /* columnwise reconstruction */
      reconstruction(image+kk1+i*m, image+kk1+i*m, image+kk1+i*m+n, n, s);

     for (i = 1; i < nn; i++) {    /* another matrix transpose */
      kk2 = i*m + kk1;
      for (jj = 0; jj < i; jj++) {
       tmp = image[kk2+jj];
       image[kk2+jj]= image[jj*m+kk1+i];
       image[jj*m+kk1+i] = tmp;
      }
     }

    }    /* end of j-loop */
   }     /* end of k-loop */
  }  /* end of if-then */
  else {
   for(i=0; i< nn; i++)
      reconstruction(image+i*m, image+i*m, image+i*m+n, n, s);

   for (i = 1; i < nn; i++) {   /* matrix transpose */
    ii = i*m;
    for (j = 0; j < i; j++) {
      tmp = image[ii+j];
      image[ii+j] = image[j*m+i];
      image[j*m+i] = tmp;
    }
   }

   for(i=0; i< nn; i++)
      reconstruction(image+i*m, image+i*m, image+i*m+n, n, s);

   for (i = 1; i < nn; i++) {   /* another matrix transpose */
    ii = i*m;
    for (j = 0; j < i; j++) {
      tmp = image[ii+ j];
      image[ii+j]= image[j*m+i];
      image[j*m+i] = tmp;
    }
   }
  }  /* end of else */

  i_wavelet_2d(image, m, 2*n, s);    /* recursion */
}
/* ------------------------------------------------------ */
decomposition(x, c, d, n, s)
double *x, *c, *d;
int n, s;
{
  int i, j, ii;
  double *tmp, sign;

  tmp = (double *) calloc (n, sizeof(double));
  for (i = 0; i < n; i++)
    tmp[i] = x[i];
  switch (s) {
  case 0:           /* cubic spline wavelet */
    for (i = 0; i < n/2; i++) {
      ii = i << 1;
      c[i] = d[i] = 0;
      sign = -1;
      for (j = -8; j <= 8; j++) {
        c[i] += spline[abs(j)] * tmp[(ii+j+n)%n];
        d[i] += sign * spline[abs(j)] * tmp[(ii+1-j+n)%n];
        sign = -sign;
      }
    }
    break;
  case 1:           /* haar basis */
    for (i = 0; i < n/2; i++) {
      ii = i << 1;
      c[i] = (tmp[ii] + tmp[ii+1]) / 2;
      d[i] = (tmp[ii] - tmp[ii+1]) / 2;
    }
    break;
  case 2:           /* daubechies 4-tap compactly supported wavelet */
    for (i = 0; i < n/2; i++) {
      ii = i << 1;
      c[i] = d[i] = 0;
      sign = -1;
      for (j = 0; j < 4; j++) {
        c[i] += d4[j] * tmp[(ii+j)%n];
        d[i] += sign * d4[j] * tmp[(ii+1-j+n)%n];
        sign = -sign;
      }
    }
    break;
  case 4:           /* daubechies 8-tap compactly supported wavelet */
    for (i = 0; i < n/2; i++) {
      ii = i << 1;
      c[i] = d[i] = 0;
      sign = -1;
      for (j = 0; j < 8; j++) {
        c[i] += d8[j] * tmp[(ii+j)%n];
        d[i] += sign * d8[j] * tmp[(ii+1-j+n)%n];
        sign = -sign;
      }
    }
    break;
  case 8:           /* daubechies 16-tap compactly supported wavelet */
    for (i = 0; i < n/2; i++) {
      ii = i << 1;
      c[i] = d[i] = 0;
      sign = -1;
      for (j = 0; j < 16; j++) {
        c[i] += d16[j] * tmp[(ii+j)%n];
        d[i] += sign * d16[j] * tmp[(ii+1-j+n)%n];
        sign = -sign;
      }
    }
    break;
  default:          /* undefined scaling function */
    break;          /* should not come to here */
  }
  free((char *)tmp);
}
/* -------------------------------------------------------- */
reconstruction(x, c, d, n, s)
double *x, *c, *d;
int n, s;
{
  int i, j, ii, jj;
  double *tmp;

  tmp = (double *) calloc (n*2, sizeof(double));

  switch (s) {
  case 0:           /* cubic spline wavelet */
    for (i = 0; i < n; i++) {
      ii = i << 1;
      tmp[ii] = tmp[ii+1] = 0;
      for (j = -4; j <= 4; j++) {
        jj = j << 1;
        tmp[ii] += spline[abs(jj)] * c[(i-j+n)%n];
        if (j != 4) {
          tmp[ii] += spline[abs(jj+1)] * d[(i+j+n)%n];
          tmp[ii+1] += spline[abs(jj+1)] * c[(i-j+n)%n];
        }
        tmp[ii+1] -= spline[abs(jj)] * d[(i+j+n)%n];
      }
    }
    break;
  case 1:           /* haar basis */
    for (i = 0; i < n; i++) {
      ii = i << 1;
      tmp[ii] = c[i] + d[i];
      tmp[ii+1] = c[i] - d[i];
    }
    break;
  case 2:           /* daubechies 4-tap compactly supported wavelet */
    for (i = 0; i < n; i++) {
      ii = i << 1;
      tmp[ii] = tmp[ii+1] = 0;
      for (j = 0; j < 2; j++) {
        jj = j << 1;
        tmp[ii] += d4[jj] * c[(i-j+n)%n] + d4[jj+1] * d[(i+j)%n];
        tmp[ii+1] += d4[jj+1] * c[(i-j+n)%n] - d4[jj] * d[(i+j)%n];
      }
    }
    break;
  case 4:           /* daubechies 8-tap compactly supported wavelet */
    for (i = 0; i < n; i++) {
      ii = i << 1;
      tmp[ii] = tmp[ii+1] = 0;
      for (j = 0; j < 4; j++) {
        jj = j << 1;
        tmp[ii] += d8[jj] * c[(i-j+n)%n] + d8[jj+1] * d[(i+j)%n];
        tmp[ii+1] += d8[jj+1] * c[(i-j+n)%n] - d8[jj] * d[(i+j)%n];
      }
    }
    break;
  case 8:           /* daubechies 16-tap compactly supported wavelet */
    for (i = 0; i < n; i++) {
      ii = i << 1;
      tmp[ii] = tmp[ii+1] = 0;
      for (j = 0; j < 8; j++) {
        jj = j << 1;
        tmp[ii] += d16[jj] * c[(i-j+n)%n] + d16[jj+1] * d[(i+j)%n];
        tmp[ii+1] += d16[jj+1] * c[(i-j+n)%n] - d16[jj] * d[(i+j)%n];
      }
    }
    break;
  default:          /* undefined scaling function */
    break;          /* should not come to here */
  }

  for (i = 0; i < n*2; i++)
    x[i] = tmp[i];

  free((char *)tmp);
}
/* ------------------------------------------------------------ */
