#include <stdio.h>
#include "tokens.h"

#include <assert.h>

int main(int argc, char **argv) {
  char input[256];

  fgets(input, 256, stdin);


  char **tokens = get_tokens(input);

  assert(tokens != NULL);

  char **current = tokens;
  while (*current != NULL) {
    printf("%s\n", *current);
    ++current;
  }

  free_tokens(tokens);
  return 0;
}
