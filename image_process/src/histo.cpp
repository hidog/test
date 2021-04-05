/******************************************************************************
Copyright 1995 by the Massachusetts Institute of Technology.  All
rights reserved.

Developed by Thomas P. Minka and Rosalind W. Picard at the Media
Laboratory, MIT, Cambridge, Massachusetts, with support from BT, PLC,
Hewlett-Packard, and NEC.

This distribution is approved by Nicholas Negroponte, Director of
the Media Laboratory, MIT.

Permission to use, copy, or modify this software and its documentation
for educational and research purposes only and without fee is hereby
granted, provided that this copyright notice and the original authors'
names appear on all copies and supporting documentation.  If individual
files are separated from this distribution directory structure, this
copyright notice must be included.  For any other uses of this software,
in original or modified form, including but not limited to distribution
in whole or in part, specific prior permission must be obtained from
MIT.  These programs shall not be used, rewritten, or adapted as the
basis of a commercial software or hardware product without first
obtaining appropriate licenses from MIT.  MIT. makes no representations
about the suitability of this software for any purpose.  It is provided
"as is" without express or implied warranty.
******************************************************************************/

/******************************************************************************
This program computes a complete unnormalized histogram for each channel of
a 3-channel image. Image similarity can be computed by Euclidean distance.
It is recommended to use a color space other than RGB, such as LAB or Ohta.
******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[])
{
  Matrix image;
  int i,y,x;
  int width,height,channels;
  int **histo;
  double *p, r,g,b;

  if(argc != 4) {
    fprintf(stderr, "Usage:\n%s <width> <height> <channels> < image_file > output\n", argv[0]);
    fprintf(stderr, "To compute the histogram of a character array with the given dimensions\n");
    fprintf(stderr, "(row-major, interleaved channels).\n");
    fprintf(stderr, "Output is the concatenation of 256 bins for each channel.\n");
    exit(1);
  }

  width = atoi(argv[1]);
  height = atoi(argv[2]);
  channels = atoi(argv[3]);
  image = MatrixCreate(height, width*channels);
  MatrixReadImage(image, stdin);

  histo = Allocate(channels, int*);
  for(i=0;i<channels;i++) {
    histo[i] = Allocate(256, int);
    for(x=0;x<256;x++) histo[i][x] = 0;
  }
  p = image->data[0];
  for(y=0;y<height;y++) {
    for(x=0;x<width;x++) {
      for(i=0;i<channels;i++)
	histo[i][ (unsigned char)*p++ ]++;
    }
  }

  for(i=0;i<channels;i++) {
    for(x=0;x<256;x++) {
      printf("%d\n", histo[i][x]);
    }
  }
  exit(0);
}
