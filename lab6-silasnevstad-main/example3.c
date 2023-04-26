#include <assert.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv) {
  pid_t child = fork();
  if (child == -1) {
    perror("Error - fork failed");
    exit(1);
  }

  if (child == 0) {
    printf("In child. Running 'ls -l' and writing the output to example3.txt\n");

    // Close standard out
    if (close(1) == -1) {
      perror("Error closing stdout");
      exit(1);
    }

    // Create the file, truncate it if it exists
    int fd = open("example3.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // The open file should replace standard out
    assert(fd == 1);

    if (execlp("ls", "ls", "-l", NULL) == -1) {
      perror("Error - execlp failed");
      exit(1);
    }

  }
  
  printf("In parent. Waiting for child %d to finish...\n", child);
  int status;
  pid_t exited_child = waitpid(child, &status, 0);
  printf("Child %d exited with status %d\n", exited_child, WEXITSTATUS(status));

  return 0;
}
