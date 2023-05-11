#include "shm_utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define RETURN_IF_NONZERO(value)                                               \
  {                                                                            \
    char c = value;                                                            \
    if (c) {                                                                   \
      return c;                                                                \
    }                                                                          \
  }

struct shm_config {
  int id;
  char (*addr)[];
  int grid_width;
  int grid_height;
};

struct shm_config config = {
    .addr = (void *)-1,
    .id = -1,
};

void shm_teardown(void) {
  if (config.addr != ((void *)-1))
    shmdt(config.addr);

  if (config.id != -1)
    shmctl(config.id, IPC_RMID, 0);
}

int shm_initialize(const unsigned int height, const unsigned int width) {
  config.grid_height = height;
  config.grid_width = width;

  // registro callback da eseguire durante la terminazione del programma
  if (atexit(shm_teardown) < 0) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  config.id = shmget(IPC_PRIVATE, sizeof(char) * height * width,
                     S_IRWXU | S_IRWXG | S_IRWXO);

  if (config.id == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  config.addr = shmat(config.id, NULL, 0);

  if (config.addr == (void *)-1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return 0;
}

int shm_grid_full(void) {
  int width = config.grid_width, height = config.grid_height;
  char(*grid)[width] = config.addr;

  for (int i = 0; i < width - 1; i++) {
    // check if there is an empty column, i.e. the last element is 0
    if (grid[i][height - 1] == 0)
      return 0;
  }

  return -1;
}

void shm_reset_board(void) {
  memset(config.addr, 0, config.grid_height * config.grid_width);
}

char check_horizontal(int width, int height, char (*grid)[width]) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j <= height - 4; j++) {
      if (grid[i][j] == 0)
        continue;

      if (grid[i][j] == grid[i + 1][j] && grid[i][j] == grid[i + 2][j] &&
          grid[i][j] == grid[i + 3][j]) {
        return grid[i][j];
      }
    }
  }

  return 0;
}

char check_vertical(int width, int height, char (*grid)[width]) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j <= height - 4; j++) {
      if (grid[i][j] == 0)
        continue;

      if (grid[i][j] == grid[i][j + 1] && grid[i][j] == grid[i][j + 2] &&
          grid[i][j] == grid[i][j + 3]) {
        return grid[i][j];
      }
    }
  }

  return 0;
}

char check_diagonal(int width, int height, char (*grid)[width]) {
  for (int i = 0; i <= width - 4; i++) {
    for (int j = height - 1; j >= 3; j--) {
      if (grid[i][j] == 0)
        continue;

      if (grid[i][j] == grid[i + 1][j - 1] &&
          grid[i][j] == grid[i + 2][j - 2] &&
          grid[i][j] == grid[i + 3][j - 3]) {
        return grid[i][j];
      }
    }
  }
  return 0;
}

char check_back_diagonal(int width, int height, char (*grid)[width]) {
  for (int i = 0; i <= width - 4; i++) {
    for (int j = 0; j <= height - 4; j++) {
      if (grid[i][j] == 0)
        continue;

      if (grid[i][j] == grid[i + 1][j + 1] &&
          grid[i][j] == grid[i + 2][j + 2] &&
          grid[i][j] == grid[i + 3][j + 3]) {
        return grid[i][j];
      }
    }
  }

  return 0;
}

char shm_check_game(void) {
  RETURN_IF_NONZERO(
      check_horizontal(config.grid_width, config.grid_height, config.addr));
  RETURN_IF_NONZERO(
      check_vertical(config.grid_width, config.grid_height, config.addr));
  RETURN_IF_NONZERO(
      check_diagonal(config.grid_width, config.grid_height, config.addr));
  RETURN_IF_NONZERO(
      check_back_diagonal(config.grid_width, config.grid_height, config.addr));

  return 0;
}
