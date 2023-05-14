#include "libconnect4.h"
#include <assert.h>

int main(void) {
  assert(sem_lib_getnum(0) == 2);
  assert(sem_lib_getnum(1) == 4);

  return 0;
}
