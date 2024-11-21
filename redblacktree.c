#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "redblacktree.h"

// Set memory space to 0 and init freelist
// Caller should reserve PGSIZE memory space for red black tree.
void
rbtinit(struct redblacktree *rbt)
{
  int i;

  memset(rbt, 0, PGSIZE);

  // init free node list
  for(i = 0; i < PGSIZE - 1; i++){
    rbt->nodes[i].next = &rbt->nodes[i + 1];
  }
  rbt->freelist = rbt->nodes;
}

// Mark given node n as most recently used.
// rbt->head should not be 0
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
      n = n->child[RB_LEFT];
    } else if(key == n->key){
      if(update) markmru(rbt, n);
      return n;
    } else if(key > n->key){
      n = n->child[RB_RIGHT];
    }
  }

  return n;
}

// Delete node with the given address n from red black tree.
static struct rbnode*
rbtdeletenode(struct redblacktree *rbt, struct rbnode* n)
{
  return 0;
}

// Take node in freelist or recycle least recently used node.
// And initialize with given parameters.
static struct rbnode*
rbt_node_alloc(struct redblacktree *rbt, enum RBCOLOR color, int key, int val)
{
  struct rbnode* freenode;

  if(rbt->freelist){
    freenode = rbt->freelist;
    rbt->freelist = freenode->next;
  } else {
    freenode = rbtdeletenode(rbt, rbt->head->prev);
  }

  freenode->color = color;
  freenode->key = key;
  freenode->val = val;
  freenode->parent = 0;
  freenode->child[RB_LEFT] = 0;
  freenode->child[RB_RIGHT] = 0;
  freenode->next = freenode;
  freenode->prev = freenode;

  return freenode;
}

// Insert node to red black tree
// Inserted node is marked as most recently used node.
// If there is no empty node, replace least recently used node.
struct rbnode*
rbtinsert(struct redblacktree *rbt, int key, int val)
{
  return 0;
}

// Delete node from red black tree.
struct rbnode*
rbtdelete(struct redblacktree *rbt, int key)
{
  return 0;
}
