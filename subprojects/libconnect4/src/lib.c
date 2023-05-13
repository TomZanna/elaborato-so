#include "libconnect4.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int parse_uint(unsigned int *const output, const char *const string) {
  char *parsed_string_end;

  unsigned long result = strtoul(string, &parsed_string_end, 10);

  if (errno == ERANGE || result > UINT_MAX || *parsed_string_end != '\0')
    return -1;

  *output = (unsigned int)result;
  return 0;
}

int sem_getnum(int player_num) { return (player_num + 1) * 2; }
