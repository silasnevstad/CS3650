// Take a look at some of the previous examples to create your own program!
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {

  char* ps_uf[16];
  ps_uf[0] = "/bin/ps";
  ps_uf[1] = "-uf";
  ps_uf[2] = NULL;

  char* echo[16];
  echo[0] = "/bin/echo";
  echo[1] = " --help";
  echo[2] = NULL;

  char* nl_b_a[16];
  nl_b_a[0] = "/bin/nl";
  nl_b_a[1] = "-b";
  nl_b_a[2] = "a";
  nl_b_a[3] = "example1.c";
  nl_b_a[4] = NULL;

  char** children[3][1] = {ps_uf, echo, nl_b_a};
  pid_t pids[3];

  for (int i = 0; i < 3; i++) {
    if ((pids[i] = fork()) == 0) {
      execve(*children[i][0], *children[i], NULL);
      exit(2);
    } else if (pids[i] > 0) {
      wait(NULL);
      printf("Code completed.");
    }
  }
  return 0;
}
