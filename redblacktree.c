#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "redblacktree.h"

// n->parent should not be 0
static inline enum RBCHILD
child_dir(struct rbnode *n)
{
  if(n->parent->child[RB_LEFT] == n) return RB_LEFT;
  return RB_RIGHT;
}

static inline struct rbnode*
sibling(struct rbnode *n)
{
  if(n->parent == 0) return 0;

  return n->parent->child[1 - child_dir(n)];
}

// child used in rotation should not be 0
// rotate(n, RB_LEFT): left rotate
// rotate(n, RB_RIGHT): right rotate
static void
rotate(struct rbnode *n, enum RBCHILD dir)
{
  struct rbnode *parent;
  struct rbnode *child;
  struct rbnode *innergrand;

  dir = 1 - dir;
  parent = n->parent;
  child = n->child[dir];
  innergrand = child->child[1 - dir];

  child->parent = parent;
  if(parent) parent->child[child_dir(n)] = child;

  child->child[1 - dir] = n;
  n->parent = child;

  n->child[dir] = innergrand;
  if(innergrand != 0) innergrand->parent = n;
}

static struct rbnode*
binary_search(
  struct redblacktree *rbt,
  int key,
  struct rbnode **pp,
  enum RBCHILD *dir
) {
  struct rbnode* n;

  n = rbt->root;
  while(n != 0){
    if(key < n->key){
      if(n->child[RB_LEFT] == 0){
        if(pp) *pp = n;
        if(dir) *dir = RB_LEFT;
        return 0;
      }
      n = n->child[RB_LEFT];
    } else if(key == n->key){
      return n;
    } else if(key > n->key){
      if(n->child[RB_RIGHT] == 0){
        if(pp) *pp = n;
        if(dir) *dir = RB_RIGHT;
        return 0;
      }
      n = n->child[RB_RIGHT];
    }
  }

  return n;
}

// Set memory space to 0 and init freelist
// Caller should reserve PGSIZE memory space for red black tree.
void
rbtinit(struct redblacktree *rbt)
{
  int i;

  memset(rbt, 0, PGSIZE);

  // init free node list
  for(i = 0; i < RBTREE_ENT - 1; i++){
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

  n = binary_search(rbt, key, 0, 0);
  if(n && update) markmru(rbt, n);

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

static void
rbt_insert_fix(struct redblacktree *rbt, struct rbnode *n)
{
  struct rbnode *parent;
  struct rbnode *uncle;
  struct rbnode *grand;

  while(1){
    parent = n->parent;
    grand = parent ? parent->parent : 0;
    uncle = sibling(parent);

    if(parent == 0){
      n->color = RB_BLACK;
      break;
    } else if(parent->color == RB_BLACK){
      break;
    } else if(uncle && uncle->color == RB_RED){
      // parent->color == RB_RED implies grand != 0 && grand->color == RB_BLACK
      parent->color = RB_BLACK;
      uncle->color = RB_BLACK;
      grand->color = RB_RED;
      n = grand;
    } else {
      // uncle is leaf node or uncle->color == RB_BLACK
      // If n is inner grandchild of grand, make n the outer grandchild of grand
      // via rotate tree at parent and switch role of parent and n
      if(child_dir(n) != child_dir(parent)){
        rotate(parent, child_dir(parent));
        n = parent;
        parent = n->parent;
      }
      // n is outer grandchild of grand
      parent->color = RB_BLACK;
      grand->color = RB_RED;
      rotate(grand, 1 - child_dir(parent));
      break;
    }
  }
}

// Insert node to red black tree
// Inserted node is marked as most recently used node.
// If there is no empty node, replace least recently used node.
// If there is a node that has duplicate key,
// only mark that duplicate node as most recently used.
struct rbnode*
rbtinsert(struct redblacktree *rbt, int key, int val)
{
  struct rbnode *freenode;
  struct rbnode *n;
  struct rbnode *p;
  enum RBCHILD childpos;

  if(rbt->root == 0){
    freenode = rbt_node_alloc(rbt, RB_BLACK, key, val);
    rbt->head = freenode;
    rbt->root = freenode;
    return freenode;
  }

  // search insert position
  n = binary_search(rbt, key, &p, &childpos);
  if(n){
    markmru(rbt, n);
    return n;
  }

  freenode = rbt_node_alloc(rbt, RB_RED, key, val);
  freenode->parent = p;
  p->child[childpos] = freenode;
  markmru(rbt, freenode);

  rbt_insert_fix(rbt, freenode);

  return freenode;
}

// Delete node from red black tree.
struct rbnode*
rbtdelete(struct redblacktree *rbt, int key)
{
  return 0;
}
