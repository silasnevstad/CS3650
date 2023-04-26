#ifndef _DEBUG_H
#define _DEBUG_H

#ifndef SHUSH
#include <stdio.h>
#define debug_printf(...) \
  fprintf(stderr, "DEBUG: " __VA_ARGS__)
#else
#define debug_printf(...)
#endif

#endif /* ifdef _DEBUG_H */
