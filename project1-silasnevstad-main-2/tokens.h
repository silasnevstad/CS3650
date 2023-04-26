#ifndef _TOKENS_H
#define _TOKENS_H

/**
 * Split the given string into tokens.
 *
 * The array of tokens obtained using this function can be freed using 
 * free_tokens().
 *
 * @param input input string
 *
 * @return a null-terminated heap-allocated array of token strings
 */
char **get_tokens(const char *input);

/**
 * Free the given array of tokens created using get_tokens().
 *
 * Both the array and each of the non-null pointers need to be allocated
 * using malloc.
 *
 * @param tokens null-terminated heap-allocated array of tokens
 */
void free_tokens(char **tokens);

#endif /* _TOKENS_H */
