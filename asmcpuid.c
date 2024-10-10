#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  uint cpuid_buf[4] = {0};
  int val = 0;

  if(argc != 2){
    printf(0, "usage: %s cpuid_code\n", argv[0]);
    exit();
  }

  val = atoi(argv[1]);

  asm volatile("movl %[val], %%eax\n"
                "cpuid\n"
                "movl %%eax, (%[buf])\n"
                "movl %%ebx, 4(%[buf])\n"
                "movl %%ecx, 8(%[buf])\n"
                "movl %%edx, 12(%[buf])\n" :
                :
                [val] "g" (val), [buf] "g" (cpuid_buf) :
                "eax", "ebx", "ecx", "edx"
  );

  printf(0, "cpuid of %d..\n", val);
  for(int i = 0; i < 4; i++)
    printf(0, "%x\n", cpuid_buf[i]);
  
  exit();
}

