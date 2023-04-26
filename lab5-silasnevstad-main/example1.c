#include <stdio.h>
#include <sys/types.h>  // Includes some new data types (e.g. pid_t)
#include <unistd.h>  // This is a new library that includes various unix system calls.

int main(int argc, char **argv) {

  pid_t pid;
  int x = 1;

  // pid returns the child process id.
  // 0 if it is the child
  // >0 is the process id of the child which the parent recieves
  pid = fork();

  if (pid == 0) {
    printf("child: x = %d\n", ++x);
    return 0;  
  }

  // Parent
  printf("parent: x = %d\n", --x);

  return 0;
}
