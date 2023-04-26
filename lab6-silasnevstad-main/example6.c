#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char **argv) {
    int pipe_fds[2]; // the pipe system call creates two file descriptors in the 2-element
                     // array given as argument

    assert(pipe(pipe_fds) == 0); // returns 0 on success

    int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
    int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipt

    int child_pid = fork();

    if (child_pid > 0) { // in parent
      close(read_fd); // close the other end of the pipe

      // replace stdout with the write end of the pipe
      if (close(1) == -1) {
        perror("Error closing stdout");
        exit(1);
      }
      assert(dup(write_fd) == 1);
    
      if (execlp("ls", "ls", "/bin", NULL) == -1) {
        perror("Error - execlp failed");
        exit(1);
      }
    }
    else if (child_pid == 0) { // in child
      close(write_fd); // close the other end of the pipe

      // replace stdin with the read end of the pipe
      if ( close(0) == -1) {
        perror("Error closing stdin");
        exit(1);
      }
      assert(dup(read_fd) == 0);

      if (execlp("nl", "nl", NULL)  == -1) {
        perror("Error - execlp failed");
        exit(1);
      }
    }
    else {
      perror("Error - fork failed");
      exit(1);
    }

    return 0;
}
