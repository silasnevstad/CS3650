#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>

#define CAP_STEP 256

static int tokens_size;
static int tokens_capacity; // initial capacity (should be enough for most applications)
static char **tokens = NULL;

static int read_string(const char *input, char *buf);
static void init_tokens();
static void add_token(const char *token);


char **get_tokens(const char *input) {
  char buf[256];
  unsigned int i = 0;
  unsigned int b = 0;

  init_tokens();

  while (input[i] != 0) {
    switch (input[i]) {
      case ' ':
      case '\n':
      case '\t':
        if (b > 0) {
          buf[b] = '\0';
          add_token(buf);
          b = 0;
        }
        break;
      case '(':
      case ')':
      case '&':
      case '>':
      case '<':
      case '|':
      case ';':
        if (b > 0) {
          buf[b] = '\0';
          add_token(buf);
          b = 0;
        }
        buf[0] = input[i];
        buf[1] = '\0';
        add_token(buf);
        break;
      case '"':
        ++i; // skip the quote
        unsigned int len = read_string(&input[i], &buf[b]);
        b += len;
        i += len;
        break;
      default:
        buf[b] = input[i];
        ++b;
    }
    ++i;
  }

  // add the last remaining token, if any
  if (b > 0) {
    buf[b] = 0;
    add_token(buf);
  }

  return tokens;
}

void free_tokens(char **tokens) {
  char **iter = tokens;

  while (*iter != NULL) {
    free(*iter);
    ++iter;
  }

  free(tokens);
}

int read_string(const char *input, char *buf) {
  unsigned int bytes = 0;
  while (*input && *input != '"') {
    *buf = *input;
    ++bytes;
    ++buf;
    ++input;
  }
  *buf = '\0';

  return bytes;
}

void init_tokens() {
  tokens_size = 0;
  tokens_capacity = CAP_STEP;
  tokens = malloc(sizeof(char *) * tokens_capacity);
  assert(tokens != NULL);
  tokens[0] = NULL;
}

void grow_tokens() {
  tokens_capacity += CAP_STEP;
  tokens = realloc(tokens, sizeof(char *) * tokens_capacity);
  assert(tokens != NULL);
}

int tokens_full() {
  return tokens_size + 1 == tokens_capacity;
}

void add_token(const char* token) {
  assert(token != NULL);
  if (tokens_full())
    grow_tokens();

  char *new = strdup(token);
  tokens[tokens_size] = new;
  ++tokens_size;
  tokens[tokens_size] = NULL;
}


