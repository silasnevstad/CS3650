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
    
    if (child_pid > 0) {
        // close the other end of the pipe
        if (close(read_fd) == -1) {
          perror("Error closing read_fd");
          exit(1);
        }
        // write a simple message to the pipe
        char *msg = "Hello, pipe!\n";
        printf("%d: Writing \"%s\" to the pipe.\n", getpid(), msg);
        assert(write(write_fd, msg, strlen(msg)) == strlen(msg)); 
        
        if (close(write_fd)  == -1) {
          perror("Error closing write_fd");
          exit(1);
        }
        waitpid(child_pid, 0, 0);
    }
    else if (child_pid == 0) {
        // close the other end of the pipe
        if (close(write_fd) == -1) {
          perror("Error closing write_fd");
          exit(1);
        }
        // read the message from the pipe
        char buf[100];
        int len = read(read_fd, buf, 100);
        if (len == -1) {
          perror("Error reading from read_fd");
          exit(1);
        }
        buf[len] = 0;

        printf("%d: Read:\n", getpid());
        assert(write(1, buf, strlen(buf)) == strlen(buf)); // write the data we've read to standard output
        if (close(read_fd) == -1) {
          perror("Error closing read_fd");
          exit(1);
        }
    }
    else {
        perror("Error - pipe failed");
        exit(1);
    }

    return 0;
}
