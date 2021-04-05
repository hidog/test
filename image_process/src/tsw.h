typedef struct tree_node {
  double energy;
  struct tree_node *children[ 4];
} tree_node;

typedef tree_node *Tree;
#define DO_CHILDREN(i) int i;for(i=0;i<4;i++)
