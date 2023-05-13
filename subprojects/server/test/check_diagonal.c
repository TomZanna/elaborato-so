#include "shm_utils.h"
#include <assert.h>

#define WIDTH 5
#define HEIGHT 5

void check_diagonal(void) {
  // clang-format off
  char grid[WIDTH][HEIGHT] = {
      {0, 0, 0, 0, 'a'}, 
      {0, 0, 0, 'a', 0}, 
      {0, 0, 'a', 0, 0},
      {0, 'a', 0, 0, 0}, 
      {0, 0, 0, 0, 0},
  };
  // clang-format on

  assert(_check_diagonal(WIDTH, HEIGHT, grid) == 'a');
}

void check_back_diagonal(void) {
  // clang-format off
  char grid[WIDTH][HEIGHT] = {
      {'a', 0, 0, 0, 0},
      {0, 'a', 0, 0, 0},
      {0, 0, 'a', 0, 0},
      {0, 0, 0, 'a', 0},
      {0, 0, 0, 0, 0},
  };
  // clang-format on

  assert(_check_back_diagonal(WIDTH, HEIGHT, grid) == 'a');
}

int main(void) {
  check_diagonal();
  check_back_diagonal();

  return 0;
}
