#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int createdFile = open("sorted_tail.txt", O_CREAT | O_WRONLY, 0666);
    int originalFile = open("many_words.txt", O_RDONLY, 0765);

    if (originalFile == -1) {
      perror("bad file!");
      exit(1);
    }

    if (createdFile == -1) {
      perror("bad file!");
      exit(1);
    }

    int pipe_fds[2]; // the pipe system call creates two file descriptors in the 2-element
                     // array given as argument

    assert(pipe(pipe_fds) == 0); // returns 0 on success

    int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
    int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipe

    int child_pid = fork();

    if (child_pid > 0) { // in parent
      close(read_fd);

      if (close(0) == -1) {
        perror("error closing stdout");
        exit(1);
      }
      assert(dup(originalFile) == 0);

      if (close(1) == -1) {
        perror("error closing stdout");
        exit(1);
      }
      assert(dup(write_fd) == 1);

      if (execlp("sort", "sort", NULL) == -1) {
        perror("error - execlp failed.");
        exit(1);
      }
    }
    else if (child_pid == 0) { // in child
      close(write_fd);

      if (close(0) == -1) {
        perror("error closing stdin");
        exit(1);
      }
      assert(dup(read_fd) == 0);

      if (close(1) == -1) {
        perror("error closing stdout");
      }
      assert(dup(createdFile) == 1);

      if (execlp("tail", "tail", NULL) == -1) {
         perror("Error - execlp failed.");
         exit(1);
      }
    }
    else {
      perror("Error - fork failed");
      exit(1);
    }

    return 0;
}
