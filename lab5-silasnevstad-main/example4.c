#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h> // Library with the 'wait' system call.
#include <unistd.h>

int main(int argc, char **argv) {

  // Store the status here
  int child_status;

  // Another way to 'fork()' a child process without storing the pid
  // The parent process will begin executing from the else block.
  if (fork() == 0) {
    printf("Child runs!\n");
    exit(0); // and exits properly
  } 
  else {
    printf("Hello from parent\n");
    // Look at 'man wait' to uncover how child_status is used to hold the child's exit status 
    wait(&child_status);

    // If we want to do something with the child_status here is an example.
    // Typically we can wait(NULL), but the parent may want
    // to capture how the child exited (maybe it exited with some error code).
    // e.g. change 'exit(0)' to 'exit(5)' in the block of code executed by
    // the child.
    if (WIFEXITED(child_status)) {
      // if child exited normally, print its exit status
      printf("Child exited with the following child_status %d\n", WEXITSTATUS(child_status));
    }
    printf("Child always is terminated by this point--parent resuming\n");
  }

    printf("This always prints last\n");

    // It is important to note that our parent should always wait for all
    // of its child processes to exit before proceeding.
    // If the parent ends first(i.e. forgetting to call wait), the
    // child process is 'orphaned' and becomes a zombie process that
    // cannot be otherwise terminated.

    return 0;
}
