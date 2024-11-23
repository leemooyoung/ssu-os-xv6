#include "types.h"
#include "defs.h"
#include "mmu.h"
#include "redblacktree.h"

// Linked list operations

// insert n1 to previous of n2
static inline void
list_insert(struct rbnode *n1, struct rbnode *n2)
{
  n1->next = n2;
  n1->prev = n2->prev;
  n1->next->prev = n1;
  n1->prev->next = n1;
}

// delete n from list
static inline void
list_delete(struct rbnode *n)
{
  n->next->prev = n->prev;
  n->prev->next = n->next;
  // n->next = n;
  // n->prev = n;
}

// Binary tree operations

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

// Extended operations

// Mark given node n as most recently used.
// rbt->head should not be 0
static void
markmru(struct redblacktree *rbt, struct rbnode *n)
{
  list_delete(n);
  list_insert(n, rbt->head);
  rbt->head = n;
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

// Increase black node number of paths that pass n, and delete n
// color of n should be black and only have leaf node as child
static void
rbt_delete_fix(struct redblacktree *rbt, struct rbnode *n)
{
  struct rbnode *p;
  struct rbnode *s;
  struct rbnode *cn;
  struct rbnode *dn;
  enum RBCHILD dir;
  struct rbnode *d;

  d = n;

  while(n->parent){
    dir = child_dir(n);
    p = n->parent;
    s = sibling(n);
    cn = s->child[dir];
    dn = s->child[1 - dir];

    if(s->color == RB_RED){ // case 3
      // colors of p, cn and dn are black
      // cn, dn is not leaf because black height should be same
      rotate(p, dir);
      p->color = RB_RED;
      s->color = RB_BLACK;
    } else if(dn != 0 && dn->color == RB_RED){ // case 6
      // color of s is black and color of cn doesn't matter
      rotate(p, dir);
      s->color = p->color;
      p->color = RB_BLACK;
      dn->color = RB_BLACK;
      break;
    } else if(cn != 0 && cn->color == RB_RED){ // case 5
      // s black, dn black
      rotate(s, 1 - dir);
      s->color = RB_RED;
      cn->color = RB_BLACK;
    } else if(p->color == RB_RED){ // case 4
      s->color = RB_RED;
      p->color = RB_BLACK;
      break;
    } else { // case 2
      // colors of p, s, cn and dn are black
      s->color = RB_RED;
      n = p; // iterate 1 black level higher
    }
  }

  d->parent->child[child_dir(d)] = 0;
  d->next = rbt->freelist;
  rbt->freelist = d;
}

// Delete node with the given address n from red black tree.
static void
rbt_node_delete(struct redblacktree *rbt, struct rbnode *n)
{
  struct rbnode *successor;

  // If n has two child, replace n with successor
  // which is leftmost node of right subtree of n,
  // and delete successor instead
  if(n->child[RB_LEFT] && n->child[RB_RIGHT]){
    // find successor
    successor = n->child[RB_RIGHT];
    while(successor->child[RB_LEFT])
      successor = successor->child[RB_LEFT];

    // insert n in place of successor in linked list
    list_delete(n);
    list_insert(n, successor);
    list_delete(successor);
    // replace key and val of n to successor's
    n->key = successor->key;
    n->val = successor->val;
    // delete successor instead
    n = successor;
  }

  // n has 0 child
  if(n->child[RB_LEFT] == 0 && n->child[RB_RIGHT] == 0){
    if(rbt->root == n){
      rbt->root = 0;
      rbt->head = 0;
    } else if(n->color == RB_RED){
      n->parent->child[child_dir(n)] = 0;
      list_delete(n);
    } else {
      // n->color == RB_BLACK
      rbt_delete_fix(rbt, n);
    }
  } else { // n has 1 child. color of n is RB_BLACK, color of child is RB_RED
    successor = n->child[RB_LEFT];
    n->child[RB_LEFT] = 0;
    if(successor == 0){
      successor = n->child[RB_RIGHT];
      n->child[RB_RIGHT] = 0;
    }

    list_delete(n);
    list_insert(n, successor);
    list_delete(successor);
    n->key = successor->key;
    n->val = successor->val;

    n = successor; // free successor instead
  }

  n->next = rbt->freelist;
  rbt->freelist = n;
}

// Take node in freelist or recycle least recently used node.
// And initialize with given parameters.
static struct rbnode*
rbt_node_alloc(struct redblacktree *rbt, enum RBCOLOR color, int key, int val)
{
  struct rbnode* freenode;

  if(rbt->freelist == 0)
    rbt_node_delete(rbt, rbt->head->prev);

  freenode = rbt->freelist;
  rbt->freelist = freenode->next;

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

// Red black tree API

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
int
rbtdelete(struct redblacktree *rbt, int key, int *val)
{
  struct rbnode *n;

  n = binary_search(rbt, key, 0, 0);
  if(n){
    *val = n->val;
    rbt_node_delete(rbt, n);
    return 0;
  }

  return -1;
}
