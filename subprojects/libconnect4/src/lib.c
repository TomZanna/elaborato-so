#include "libconnect4.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

int parse_uint(unsigned int *const output, const char *const string) {
  char *parsed_string_end; // puntatore al primo carattere non analizzato

  // decodifica il numero
  unsigned long result = strtoul(string, &parsed_string_end, 10);

  // errore se non ha analizzato tutta la stringa o è oltre l'intervallo
  if (errno == ERANGE || result > UINT_MAX || *parsed_string_end != '\0')
    return -1;

  *output = (unsigned int)result;
  return 0;
}
