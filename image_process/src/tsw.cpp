/*****************************************************************************
Tree-structured wavelet decomposition of a textured image.
Based on code from Tianhorng Chang and the paper,
 
T. Chang and C.-C. J. Kuo,
"Texture Analysis and Classification with Tree-Structured Wavelet Transform",
IEEE Trans. Image Processing, vol. 2, no. 4, pp. 429-441, 1993.

A fairly complex algorithm is needed to compare trees (not Euclidean distance).
******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include <cstdlib>
#include <cstring>
#include "tsw.h"

#define SCALE 2

void MakeTree(Matrix image, Tree *root, int levels);
void WriteTree(Tree root);
void WriteLeaves(Tree root);

void main(int argc, char *argv[])
{
  Matrix image;
  double mean;
  Tree root;
  int levels, width, height;
  FILE *fp;

  if(argc < 4) {
    fprintf(stderr, "Usage:\ntsw <width> <height> <image_file>\n");
    fprintf(stderr, "To compute the tree-structured wavelet tree of a character array.\n");
    fprintf(stderr, "The quadtree of coefficients is stored in prefix form.\n");
    exit(0);
  }
  
  width = atoi(argv[1]);
  height = atoi(argv[2]);

  image = MatrixCreate(width, height);
  fp = fopen(argv[3], "r");
  if(!fp) { perror(""); exit(1); }
  MatrixReadImage(image, fp);
  fclose(fp);
  mean = MatrixMean(image);
  MatrixAddValue(image, -mean);

  levels = 4;
  MakeTree(image, &root, levels);
  WriteTree(root);
}

void MakeTree(Matrix image, Tree *root, int levels)
{
  Matrix subimage;
  int yh, xh;
  static int count=0;
  char fname[40];

  if(levels == 0) {
    *root = NULL;
    return;
  }
  
  *root = Allocate(1, tree_node);
  subimage = MatrixCopy(image);
  MatrixApplyFunc(subimage, fabs);
  (*root)->energy = MatrixMean(subimage);
  MatrixFree(subimage);

  wavelet_2d(image->data[0], image->height, image->width, 1, SCALE);

  yh = image->height/2;
  xh = image->width/2;

  subimage = MatrixSub(image, 0,0, yh-1,xh-1);
  MakeTree(subimage, &(*root)->children[0], levels-1);
  MatrixFree(subimage);

  subimage = MatrixSub(image, 0,xh, yh-1,image->width-1);
  MakeTree(subimage, &(*root)->children[1], levels-1);
  MatrixFree(subimage);

  subimage = MatrixSub(image, yh,0, image->height-1,xh-1);
  MakeTree(subimage, &(*root)->children[2], levels-1);
  MatrixFree(subimage);

  subimage = MatrixSub(image, yh,xh, image->height-1,image->width-1);
  MakeTree(subimage, &(*root)->children[3], levels-1);
  MatrixFree(subimage);
}

void WriteLeaves(Tree root)
{
  int is_leaf;

  is_leaf = 1;
  {DO_CHILDREN(i) if(root->children[i]) is_leaf = 0; }

  if(is_leaf) {
    printf("%lg\n", root->energy);
  }
  else {
    {DO_CHILDREN(i) WriteLeaves(root->children[i]); }
  }
}

void WriteTree(Tree root)
{
  if(!root) return;
  printf("%lg\n", root->energy);
  {DO_CHILDREN(i) WriteTree(root->children[i]); }
}
