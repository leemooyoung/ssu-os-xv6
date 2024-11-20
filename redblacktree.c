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

// Mark as most recently used.
static void
markmru(struct redblacktree *rbt, struct rbnode *n)
{
  // delete node in linked list
  n->prev->next = n->next;
  n->next->prev = n->prev;
  // insert to prev of head
  n->next = rbt->head;
  n->prev = rbt->head->prev;
  n->next->prev = n;
  n->prev->next = n;
  // set n to head
  rbt->head = n;
}

// Binary search in red black tree
// If update is not 0, mark the found tree node as most recently used.
struct rbnode*
rbtsearch(struct redblacktree *rbt, int key, int update)
{
  struct rbnode* n;

  n = rbt->root;
  while(n != 0){
    if(key < n->key){
      n = n->lchild;
    } else if(key == n->key){
      if(update) markmru(rbt, n);
      return n;
    } else if(key > n->key){
      n = n->rchild;
    }
  }

  return n;
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
