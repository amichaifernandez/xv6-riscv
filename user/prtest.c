#include "kernel/types.h"
#include "user/user.h"

// Test getpriority()/setpriority() and priority inheritance.
int
main(int argc, char *argv[])
{
  int pid;

  printf("start: priority = %d\n", getpriority());

  if(setpriority(10) < 0)
    printf("setpriority(10) failed\n");
  printf("after setpriority(10): priority = %d\n", getpriority());

  // Out-of-range values should be rejected (-1) and change nothing.
  printf("setpriority(60) returned %d\n", setpriority(60));
  printf("setpriority(-1) returned %d\n", setpriority(-1));
  printf("priority still = %d\n", getpriority());

  pid = fork();
  if(pid == 0){
    // Child should inherit priority 10 from the parent.
    printf("child: priority = %d\n", getpriority());
    setpriority(20);
    printf("child: after setpriority(20) = %d\n", getpriority());
    char *args[] = { "ps", 0 };
    exec("ps", args);          // run ps as the child, at priority 20
    exit(1);
  }
  wait(0);
  printf("parent: priority = %d (unchanged by child)\n", getpriority());
  exit(0);
}
