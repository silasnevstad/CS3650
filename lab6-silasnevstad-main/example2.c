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
