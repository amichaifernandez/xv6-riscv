#include "kernel/types.h"
#include "user/user.h"

// agetest: start several CPU-bound children with different priorities at
// the same moment and report each one's response time (ticks until it
// first runs) and turnaround time (ticks until it finishes), plus averages.
// usage: agetest [work]   (amount of computation per child, default 5000)

#define N 5

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
  int prios[N] = { 5, 20, 10, 0, 15 };
  int starts[N];
  int work = 5000;
  int fd[2], start, i, status;
  int totresp = 0, totturn = 0;

  if (argc > 1)
    work = atoi(argv[1]);

  pipe(fd);
  setpriority(49);   // parent outranks the children while it sets them up

  for (i = 0; i < N; i++) {
    if (fork() == 0) {
      // Child: set its priority, then block on the pipe until the start signal.
      close(fd[1]);
      setpriority(prios[i]);
      read(fd[0], &start, sizeof(start));
      int resp = uptime() - start;     // ticks until this child first ran
      spin(work);
      int turn = uptime() - start;     // ticks until this child finished
      printf("pid %d  priority %d  response %d  turnaround %d\n",
             getpid(), prios[i], resp, turn);
      exit(resp * 1000 + turn);        // pack both numbers into the exit status
    }
  }

  // Give every child a chance to set its priority and block on the pipe.
  pause(2);

  // Start signal: send the start time to all children at once.
  start = uptime();
  for (i = 0; i < N; i++)
    starts[i] = start;
  write(fd[1], starts, sizeof(starts));
  close(fd[1]);

  // Collect results and compute averages (one decimal place).
  for (i = 0; i < N; i++) {
    wait(&status);
    totresp += status / 1000;
    totturn += status % 1000;
  }
  printf("average response %d.%d ticks, average turnaround %d.%d ticks\n",
         totresp / N, (totresp * 10 / N) % 10,
         totturn / N, (totturn * 10 / N) % 10);
  exit(0);
}
