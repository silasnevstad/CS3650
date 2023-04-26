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
    printf("In child. Running 'sort' to sort the words in words.txt\n");

    // Close standard in
    if (close(0) == -1) {
      perror("Error closing stdin");
      exit(1);
    }


    // Open the file for reading
    int fd = open("words.txt", O_RDONLY);

    // The open file should replace standard in
    assert(fd == 0);

    if (execlp("sort", "sort", NULL) == -1) {
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
