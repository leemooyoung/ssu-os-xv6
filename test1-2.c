#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(0, "start scheduler_test\n");
  if(fork() == 0){
    set_proc_info(1, 0, 0, 0, 500);
    while(1);
  }else{
    wait();
  }
  printf(0, "end of scheduler_test\n");
  exit();
}
