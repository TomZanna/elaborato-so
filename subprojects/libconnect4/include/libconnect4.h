#include <stdlib.h>

extern int ciao;

#define EXIT_ON_ERR(func)                                                      \
  if (func != 0)                                                               \
    exit(1);
