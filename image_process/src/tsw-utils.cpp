/*
  utilities for dealing with the TSW output.
  load_tree( filename, tree)
    where tree is an allocated pointer to a tree_node.
  dump_tree( tree, levels)
    levels is the height of the tree you're dumping.
*/

#include "tsw.h"

get_energy( fp, node, level)
     FILE *fp;
     tree_node *node;
     int level;
{
  int i, j;
  tree_node *temp;
  float tnrg;
  
  fscanf( fp, "%f", &tnrg);

  node->energy = tnrg;
  node->index = global_count++;

  if (level == 1) {
    for (i = 0; i < 4; i++)
      node->children[ i] = (tree_node *) NULL;
    return;
  }

  for (i = 0; i < 4; i++) {
    temp = (tree_node *) malloc( sizeof( tree_node));
    node->children[ i] = temp;
    get_energy( fp, node->children[ i], level - 1);
  }
  return;
}

load_tree( filename, tree)
     char *filename;
     tree_node *tree;
{
  int i, j;
  FILE *fp;

  fp = fopen( filename, "r");
  if (fp == NULL) {
    printf("open error.\n");
    exit(-1);
  }

  global_count = 0;
  get_energy( fp, tree, 4);
  fclose( fp);

}

dump_leaves( stream, node, level)
     FILE *stream;
     tree_node *node;
     int level;
{

  int i;

  if (level == 1) {
    fprintf( stream, "%f\n", node->energy);
    return;
  }
  for (i = 0; i < 4; i++)
    dump_leaves( stream, node->children[ i], level - 1);
}

dump_tree( stream, node, level)
     FILE *stream;
     tree_node *node;
     int level;
{
  int i;

  fprintf(stream, "%f\n", node->energy);
    if (node->children[ 0] != NULL) {
      for (i = 0; i < 4; i++)
	dump_tree( stream, node->children[ i], level - 1);
    }
}
dump_tree_mask( stream, node, level)
     FILE *stream;
     tree_node *node;
     int level;
{
  int i;

  if ( node->energy < 0.0)
    fprintf( stream, "0\n");
  else
    fprintf( stream, "1\n");
  if (node->children[ 0] != NULL) {
    for (i = 0; i < 4; i++)
      dump_tree_mask( stream, node->children[ i], level - 1);
  }
}

double find_highest( node, level, current)
     tree_node *node;
     int level;
     double *current;
{
  int i;
  double c[ 5];
  double temp = 0.0;

  if (level == 1)
    return( node->energy);
    
  for (i = 0; i < 4; i++)
    c[ i] = find_highest( node->children[ i], level - 1, current);
  c[ 4] = node->energy;
  for (i = 0; i < 5; i++)
    if (c[ i] > temp)
      temp = c[ i];
  *current = temp;
  return( temp);
}

tree_scale( node, level, factor)
     tree_node *node;
     int level;
     double factor;
{
  int i;
  node->energy *= factor;

  if (level == 1)
    return;
  
  for (i = 0; i < 4; i++)
    tree_scale( node->children[ i], level - 1, factor);
}

parse_tree( node, level, threshold, array)
     tree_node *node;
     int level;
     double threshold;
     int *array;
{
  int i;

  /* do your function here */

  if (node->energy < threshold) {
    array[ node->index] = 0;
    node->energy *= -1.0;
  }
  else
    array[ node->index] = 1;
    
  if (level == 1)
    return;
  
  for (i = 0; i < 4; i++)
    parse_tree( node->children[ i], level - 1, threshold, array);
}
  
  
  

