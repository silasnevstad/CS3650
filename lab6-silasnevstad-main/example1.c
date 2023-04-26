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
