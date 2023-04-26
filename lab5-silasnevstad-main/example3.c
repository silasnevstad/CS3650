#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {

  pid_t pid;
  int x = 1;

  // pid returns the child process id.
  // 0 if it is the child
  // >0 is the process id of the child which the parent recieves
  pid = fork();

  if (pid == 0) {
    printf("Running child forever now pid = %d\n",getpid());
    while (1) {
    }
  }
  else {
    // Run the parrent
    printf("Terminating parent pid = %d\n", getpid());
    exit(0);
  }

  return 0;
}
