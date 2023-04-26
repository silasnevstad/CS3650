# POSIX File I/O, or: (Almost) Everything is a File  

**There's a checklist quiz on Canvas for this lab: <https://northeastern.instructure.com/courses/99572/quizzes/411291>**  

In today's lab, we will experiment with the [POSIX](https://pubs.opengroup.org/onlinepubs/9699919799/) API for file operations. On Linux, these functions are implemented in terms of system calls, which you might have used when programming in assembly. When using these routines, files are represented using numerical *file descriptors*, that is, integer values `>= 0`. Since, in UNIX-like systems, everything is a file, knowing how to use these functions will give you access to more than just traditional files. This lab contains several examples that will take you from reading and writing from standard input and output, to basics of inter-process communication.


Notes:
- We expect you to use `man 2 <function>` or `man 3 <function>` if details of some functions are not clear, but also do not hesitate to ask questions!

- While submissions for this lab are individual, feel free to partner up for work and discussion.

## Example 1 - read and write

The functions [`read`](https://man7.org/linux/man-pages/man2/read.2.html) and [`write`](https://man7.org/linux/man-pages/man2/write.2.html) are the workhorses of low-level file I/O and, on Linux, directly correspond to the read and write [syscalls](https://filippo.io/linux-syscall-table/). The function `read` takes an integer file descriptor, the memory address of a buffer where to store data read from the file, and the (maximum) number of bytes to be read. Upon success, it returns the number of bytes actually read. As other functions discussed here, it will return `-1` on error. Note, `read` will return `0` if there are no more bytes to be read (and you requested more than `0` bytes). With files, this usually means that the [end-of-file](https://en.wikipedia.org/wiki/End-of-file) has been reached.

The `write` function takes pretty much the same arguments: a file descriptor, the buffer containing data to be written, and the number of bytes to be written. On success, it returns the number of bytes written, otherwise it will return `-1`.

###### File descriptors

A file descriptor is an integer (`int`), which, in many implementations, is just an index to the *file descriptor table* of a process. When a process starts, three file descriptors are open by default:

 | Descriptor | Purpose                        |
 |------------|--------------------------------|
 | 0          | standard input (stdin)         |
 | 1          | standard output (stdout)       |
 | 2          | standard error output (stderr) |


Here's an example program working with stdin and stdout using `read` and `write`.


- Copy the below code snippet and paste it into [`example1.c`](example1.c)
- Compile using `gcc -std=c11 example1.c -o example1` or simply use `make example1`
- Run the program: `./example1`
- Experiment
- Study the manpages for [`read`](https://man7.org/linux/man-pages/man2/read.2.html) and [`write`](https://man7.org/linux/man-pages/man2/write.2.html). They are simple yet powerful functions used for a wide variety of purposes (including when implementing a web crawler or browser).
- Modification idea: implement a program that echoes every line the user writes, until Ctrl-D is pressed


```c
#include <assert.h>
#include <string.h>

#include <unistd.h>

int main(int argc, char **argv) {
  char msg1[] = "What's your name?\n";
  char msg2[] = "Hello, ";
  char buffer[256];

  /* write takes:
   *  - file desriptor
   *  - a buffer with bytes to be written
   *  - the number of bytes to be written
   * it returns the number of bytes actually written
   */

  /* Write the prompt to standard out.
   *
   * Note: we use assert to verify that the number of bytes written is the 
   * number of bytes we asked to be written.
   */
  assert(write(1, msg1, strlen(msg1)) == strlen(msg1));

  /* read takes:
   *   - file desriptor
   *   - a buffer to which the read bytes will be written
   *   - number of bytes to write
   * it returns the number of bytes read
   */

  /* Read the name from standard input */
  int length = read(0, buffer, 255);
  buffer[length] = '\0'; // properly terminate the string we read into the buffer

  /* Write the hello message... */
  assert(write(1, msg2, strlen(msg2)) == strlen(msg2));

  /* ...followed by the name. */
  assert(write(1, buffer, length) == length);

  return 0;
}
```

## Example 2 - open and close

Files can be opened using the [`open`](https://man7.org/linux/man-pages/man2/open.2.html) call. It takes two arguments by default: the path and name of the file to be opened, and the *flag* that determining how the file should be opened. Do read the manpage for an explanation of the flags, but here are some basics.

At a minimum, we need to tell `open` whether we want to open the file for reading, writing, or both using `O_RDONLY`, `O_WRONLY`, or `O_RDWR`. Then, if we are opening the file for writing, we might want to specify whether we want to create it if it doesn't exist (`O_CREAT`) or if we want to truncate it to 0 bytes if it does (`O_TRUNC`). How are these flags combined? Using a clever trick: each flag is defined as an integer that, in binary, is just a single bit set to 1, e.g. 1, 2, 4, 8, 16, ... This means that the multiple flags can be set at the same time by using the bit-wise or operator `|`. For example, if `O_WRONLY` is defined as `2` and `O_CREAT` as `8` (they are most likely different), combining them yields:

`(O_WRONLY | O_CREAT) = (2 | 8) = (0b0010 | 0b1000) = 0b1010 = 10`

Important: When `O_CREAT` is used, `open` takes a third argument: the file access mode, which is a number specifying the access rights of the owner, group, and rest of the world (this is what you see when you use `ls -l` as `-rwxr-xr-x`, for example). The traditional way to specify the access mode is to use [*octal*](https://en.wikipedia.org/wiki/Octal) notation (= base 8, i.e., digits are 0-7, prefixed with `0`) as follows: 1 stands for the executable flag, 2 for write, 4 for read. To combine flags, simply add these up. The most significant digit is the owner, then the group, then the rest of the world. Example:

- `0765` specifies `rwxrw-r-x`

See [`man chmod`](https://man7.org/linux/man-pages/man1/chmod.1.html) or this [Wikipedia article](https://en.wikipedia.org/wiki/Chmod) for a thorough explanation.

- Copy the below code snippet and paste it into [`example2.c`](example2.c)
- Compile using `gcc -std=c11 example2.c -o example2` or simply use `make example2`
- Run the program: `./example2`
- Inspect the created file to confirm the behavior. Feel free to experiment.
- Study the manpage for [`open`](https://man7.org/linux/man-pages/man2/open.2.html).
- Mini-challenge idea: implement a simplified version of the [`cat`](https://man7.org/linux/man-pages/man1/cat.1.html) command

```c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char line[] = "Here's a line.\n";
  char buffer[1024];

  /* open takes
   *   - a path to the file to be opened
   *   - a set of flags, combined using the bit-wise or operator
   *     these flags specify, for example, whether a file should be open for
   *     reading, writing or both, whether it should be created if it doesn't 
   *     exist, whether it should be overwritten if it does, etc.
   *
   * See man 2 open
   *
   * Here we open example2.txt for writing, we create it if it doesn't exist, and
   * truncate it to 0 if it does. Since we are creating it, we specify that 
   * it should be created with the following access rw-r--r-- (verify using 
   * ls -l)
   */
  int fd = open("example2.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

  /* open returns -1 on failure */
  if (fd == -1) {
    perror("Error trying to open example2.txt");
    exit(1);
  }

  if (write(fd, line, strlen(line)) == -1) {
    perror("Error writing to example2.txt");
    exit(1);
  }

  /* close the file */
  if (close(fd) == -1) {
    perror("Error closing example2.txt");
    exit(1);
  }

  /* re-open the file for reading AND writing (appending) */
  fd = open("example2.txt", O_RDWR | O_APPEND);

  /* open returns -1 on failure */
  if (fd == -1) {
    perror("Error trying to open example2.txt");
    exit(1);
  }

  /* Now duplicate the contents of the file. */
  int length = read(fd, buffer, 1024);

  if (length == -1) {
    perror("Error reading from example2.txt");
    exit(1);
  }

  if (write(fd, buffer, length) == -1) {
    perror("Error writing to example2.txt");
    exit(1);
  }

  /* close the file */
  if (close(fd) == -1) {
    perror("Error closing example2.txt");
    exit(1);
  }

  return 0;
}
```

## Example 3 - Redirecting stdout

Now we get to the fun part: manipulating where the standard input reads from and where the standard output writes to. To do this, we use the 2 facts guaranteed by the system:

1. When opening files, file descriptors are assigned sequentially starting with the first available one.
2. Then a file is closed, its file descriptor is made available.

Since we can close any of the default streams, we can replace it easily with a file and then run our program.

- Copy the below code snippet and paste it into [`example3.c`](example3.c)
- Compile using `gcc -std=c11 example3.c -o example3` or simply use `make example3`
- Run the program: `./example3`
- Inspect the created file to confirm the behavior. Feel free to experiment.

```c
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
```

## Example 4 - Redirecting stdin

The same above "trick" applies to standard input.

- Copy the below code snippet and paste it into [`example4.c`](example4.c)
- Compile using `gcc -std=c11 example4.c -o example4` or simply use `make example4`
- Run the program: `./example4`
- Inspect the created file to confirm the behavior. Feel free to experiment.

```c
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
```

## Example 5 - Inter-process communication using pipes

We learned that one of the goals of a modern OS is to isolate its processes, that is, to allow multiple programs run at the same time, blissfully unaware of each other. That's useful and allows you to run several programs safely, but often processes need to talk to each other. This is why a Unix-like system offers multiple approaches to [*inter-process communication*](https://en.wikipedia.org/wiki/Inter-process_communication) (IPC). 

One of the simplest and most straightforward ones are *anonymous pipes*. An anonymous pipe is a unidirectional (fancy way to say "one-way") communication channel. A pipe has a read end and a write end and both have their own file descriptors. What does that mean? We can write to a pipe using `write` and read from it using, well, `read`. A pipe is created using the [`pipe`](https://man7.org/linux/man-pages/man2/pipe.2.html) system call. The following example creates a pipe and then forks off a child process. Since a forked process inherits all the open files (among other parental possessions), it also inherits the open pipe ends. We simply close the one we don't need and send a message from the parent to the child.

- Copy the below code snippet and paste it into [`example5.c`](example5.c)
- Compile using `gcc -std=c11 example5.c -o example5` or simply use `make example5`
- Run the program: `./example5`
- Read [`man 2 pipe`](https://man7.org/linux/man-pages/man2/pipe.2.html)
- Experiment

```c
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
```


## Example 6 - More pipes

Here we redirect standard in and out using pipes. This is how the pipe command line operator `|` is implemented. We use the old trick of closing one of the standard streams and replacing it with another "file". Since the pipe is already open when we get to this (it has to be created before a process is forked), we use the [`dup`](https://man7.org/linux/man-pages/man2/dup.2.html) system call to duplicate an existing file descriptor. Read through this example carefully and make sure you understand what's going on.

- Copy the below code snippet and paste it into [`example6.c`](example6.c)
- Compile using `gcc -std=c11 example6.c -o example6` or simply use `make example6`
- Run the program: `./example6`
- Read [`man 2 dup`](https://man7.org/linux/man-pages/man2/dup.2.html)
- Experiment


```c
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
```

## Programming Task - Putting it all together

For the programming task, in [`task.c`](task.c), implement an equivalent of the following command line:

```
$ sort < many_words.txt | tail > sorted_tail.txt
```

That is,

1. redirect standard in of `sort` to take the input from `many_words.txt` (provided),
2. redirect the stdout of `sort` to the stdin of `tail`, and
3. redirect the stdout of `tail` into a file named `sorted_tail.txt`

You should only need the system calls `pipe`, `fork`, `close`, `open`, `dup`, one of the `exec`s, and possibly one of the `wait`s. We recommend starting with the `example6.c` code and modifying it.

## Other examples

Also included is a somewhat crude implementation of the `cp` command using the POSIX system calls. See [copy.c](copy.c).

## Lab Deliverables

1. Make sure you complete all the steps on the [Lab 6 Quiz](https://northeastern.instructure.com/courses/99572/quizzes/411291) checklist.
2. Commit the source code for the 6 examples (`example1.c`, `example2.c`, `example3.c`, `example4.c`, `example5.c`, `example6.c`).
3. Commit your source code for [task.c](./task.c)
4. Upload your repo to Gradescope


## More resources to help

- On Interprocess Communication (i.e. pipe)
  - <https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lec/18/lec.html>
- Another useful tutorial on pipe, fork, etc.
  - <http://www.rozmichelle.com/pipes-forks-dups/>
  

