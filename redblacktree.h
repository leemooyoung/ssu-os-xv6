#define RBTREE_ENT 100

enum RBCOLOR { RB_RED, RB_BLACK };

enum RBCHILD { RB_LEFT, RB_RIGHT };

// The sort order is as follows
// child[RB_LEFT]->key < key < child[RB_RIGHT]->key
struct rbnode {
  struct rbnode *parent;
  struct rbnode *child[2];

  struct rbnode *next;
  struct rbnode *prev;

  enum RBCOLOR color;
  int key;
  int val;
};

// There is no sleeplock for struct redblacktree. Use the inode's one instead.
struct redblacktree {
  // Root of tree
  struct rbnode *root;

  // List of nodes, in order of most recently used.
  // head is most recently used, head->next is second most recently used.
  // and head->prev is least recently used node
  struct rbnode *head;

  // List of free nodes. only use ->next
  struct rbnode *freelist;

  struct rbnode nodes[RBTREE_ENT];
};

void rbtinit(struct redblacktree *);

struct rbnode *rbtsearch(struct redblacktree *, int, int);

struct rbnode *rbtinsert(struct redblacktree *, int, int);

int rbtdelete(struct redblacktree *, int, int *);

void rbtprint(struct redblacktree *);

int check_rbt(struct redblacktree *);
