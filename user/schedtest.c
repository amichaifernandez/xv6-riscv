#include "kernel/types.h"
#include "user/user.h"

// schedtest: start several CPU-bound children with different priorities
// at the same time, and report when each one finishes.
// usage: schedtest [work]   (amount of computation per child, default 300)

volatile int sink;   // global, so the compiler can't optimize the loop away

// Burn CPU time doing useless arithmetic.
void
spin(int work)
{
  for (int i = 0; i < work; i++)
    for (int j = 0; j < 100000; j++)
      sink = sink + j;
}

int
main(int argc, char *argv[])
{
  int prios[] = { 5, 20, 10, 0, 15 };
  int n = 5;
  int work = 300;
  int start, i;

  if (argc > 1)
    work = atoi(argv[1]);

  // Run the parent at the top priority so it creates every child
  // before any of them gets the CPU.
  setpriority(49);
  start = uptime();

  for (i = 0; i < n; i++) {
    if (fork() == 0) {
      // Child: lower its own priority, do the work, report the finish time.
      setpriority(prios[i]);
      spin(work);
      printf("pid %d  priority %d  finished at tick %d\n",
             getpid(), prios[i], uptime() - start);
      exit(0);
    }
  }

  // Parent: wait for all children.
  for (i = 0; i < n; i++)
    wait(0);
  printf("all done after %d ticks\n", uptime() - start);
  exit(0);
}
