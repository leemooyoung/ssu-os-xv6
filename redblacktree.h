#define RBTREE_ENT 100

enum RBCOLOR { RB_RED, RB_BLACK };

enum RBCHILD { RB_LEFT, RB_RIGHT };

// The sort order is as follows: lchild->key < key < rchild->key
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
  struct rbnode nodes[RBTREE_ENT];
  struct rbnode *root;
  struct rbnode *head;
  struct rbnode *freelist;
};

void rbtinit(struct redblacktree *);

struct rbnode *rbtsearch(struct redblacktree *, int, int);

struct rbnode *rbtinsert(struct redblacktree *, int, int);

struct rbnode *rbtdelete(struct redblacktree *, int);
