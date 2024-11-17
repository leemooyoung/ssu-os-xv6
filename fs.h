// On-disk file system format.
// Both the kernel and user programs use this header file.


#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

#define BPINDIRECT (BSIZE / sizeof(uint))

#define DIR_BTAB_ENT 6 // Number of Direct Block Table Entry
#define S_INDIR_BTAB_ENT 4 // Number of Single Indirect Block Table Entry
#define D_INDIR_BTAB_ENT 2 // Number of Double Indirect Block Table Entry
#define T_INDIR_BTAB_ENT 1 // Number of Triple Indirect Block Table Entry

#define BTAB_ENT \
  (DIR_BTAB_ENT + S_INDIR_BTAB_ENT + D_INDIR_BTAB_ENT + T_INDIR_BTAB_ENT)

#define NDIRECT DIR_BTAB_ENT
#define N_1_INDIRECT (S_INDIR_BTAB_ENT * BPINDIRECT)
#define N_2_INDIRECT (D_INDIR_BTAB_ENT * BPINDIRECT * BPINDIRECT)
#define N_3_INDIRECT (T_INDIR_BTAB_ENT * BPINDIRECT * BPINDIRECT * BPINDIRECT)

#define MAXFILE (NDIRECT + N_1_INDIRECT + N_2_INDIRECT + N_3_INDIRECT)

#define NINDIRECT (BSIZE / sizeof(uint))

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[BTAB_ENT];   // Data block addresses
};

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb)     ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b/BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

