#include "types.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
  int fd;
  if(argc < 4) {
    printf(1, "usage: lseek_test <filename> <offset> <string>\n");
    exit();
  }

  fd = open(argv[1], O_RDWR);
  lseek(fd, atoi(argv[2]), SEEK_SET);
  write(fd, argv[3], (int)strlen(argv[3]));
  close(fd);

  exit();
}
