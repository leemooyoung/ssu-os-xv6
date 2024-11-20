#define RBTREE_ENT 100

enum RBCOLOR { RED, BLACK };

// The sort order is as follows: lchild->key < key < rchild->key
struct rbnode {
    struct rbnode *parent;
    struct rbnode *lchild;
    struct rbnode *rchild;

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
    struct rbnode *empty;
};

void rbtinit(struct redblacktree *);

struct rbnode *rbtsearch(struct redblacktree *, int, int);

struct rbnode *rbtinsert(struct redblacktree *, int, int);

struct rbnode *rbtdelete(struct redblacktree *, int);
