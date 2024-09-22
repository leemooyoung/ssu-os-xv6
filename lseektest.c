#include "types.h"
#include "user.h"
#include "fcntl.h"

#define BUF_LEN 512

// read all file content and print
// as side effect, set file offset to SEEK_SET
// fd must be valid
void print_file(int fd)
{
  lseek(fd, 0, SEEK_SET);
  char buf[BUF_LEN + 1];
  int len = 0;

  while(1) {
    memset(buf, 0, BUF_LEN + 1);
    if((len = read(fd, buf, BUF_LEN)) <= 0) break;
    printf(1, "%s", buf);
  }

  lseek(fd, 0, SEEK_SET);

  return;
}

int main(int argc, char *argv[])
{
  int fd;
  if(argc < 4) {
    printf(1, "usage: lseek_test <filename> <offset> <string>\n");
    exit();
  }


  fd = open(argv[1], O_RDWR);

  printf(1, "Before : ");
  print_file(fd);
  printf(1, "\n");

  lseek(fd, atoi(argv[2]), SEEK_SET);
  write(fd, argv[3], (int)strlen(argv[3]));

  printf(1, "After : ");
  print_file(fd);
  printf(1, "\n");
  close(fd);

  exit();
}
