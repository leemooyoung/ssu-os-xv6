#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "redblacktree.h"

// Set memory space to 0.
// Caller should reserve PGSIZE memory space for red black tree.
// If not, calls panic.
void
rbtinit(struct redblacktree *rbt)
{
  if(sizeof(struct redblacktree) > PGSIZE)
    panic("rbtinit");

  memset(rbt, 0, PGSIZE);
}

// Binary search in red black tree
// If update is not 0, mark the found tree node as most recently used.
struct rbnode*
rbtsearch(struct redblacktree *rbt, int key, int update)
{
  return 0;
}

// Insert node to red black tree
// Inserted node is marked as most recently used node.
// If there is no empty node, replace least recently used node.
struct rbnode*
rbtinsert(struct redblacktree *rbt, int key, int val)
{
  return 0;
}

// Delete node in red black tree.
struct rbnode*
rbtdelete(struct redblacktree *rbt, int key)
{
  return 0;
}
