#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(0, "start scheduler_test\n");
  if(fork() == 0){
    set_proc_info(2, 0, 0, 0, 300);
    while(1);
  }

  if(fork() == 0){
    set_proc_info(2, 0, 0, 0, 300);
    while(1);
  }

  if(fork() == 0){
    set_proc_info(2, 0, 0, 0, 300);
    while(1);
  }

  wait();
  wait();
  wait();
  printf(0, "end of scheduler_test\n");
  exit();
}
