#include "shm_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

struct grid_conf {
  char (*addr)[];
  int width;
  int height;
  char token;
};

struct grid_conf grid = {.addr = (void *)-1};

void shm_teardown() {
  if (grid.addr != (void *)-1)
    shmdt(grid.addr);
}

int shm_attach(int shm_id, int width, int height, int player_token) {
  // registro callback da eseguire durante la terminazione del programma
  if (atexit(shm_teardown) < 0) {
    perror("Errore durante la connessione");
    return -1;
  }

  grid.token = player_token;
  grid.width = width;
  grid.height = height;
  grid.addr = shmat(shm_id, NULL, 0);

  if (grid.addr == (void *)-1) {
    perror("Errore durante la connessione");
    return -1;
  }

  return 0;
}

void shm_print_grid(void) {
  char(*const grid_addr)[grid.height] = grid.addr;

  for (int i = grid.height - 1; i >= 0; i--) {
    for (int j = 0; j < grid.width; j++) {
      printf("|%c", grid_addr[j][i] ? grid_addr[j][i] : ' ');
    }
    printf("|\n");
  }

  for (int i = 0; i < grid.width; i++)
    printf("--");
  printf("-\n");

  for (int i = 0; i < grid.width; i++)
    printf(" %i", i + 1);
  printf("\n");

  return;
}

int shm_input_valid(int column) {
  char(*const grid_addr)[grid.height] = grid.addr;

  // cerco la prima casella vuota (se c'è)
  int pos;
  for (pos = 0; pos < grid.height; pos++)
    if (grid_addr[column][pos] == 0)
      break;

  // se non ci sono casella vuote, la colonna è piena
  if (pos == grid.height) {
    return -1;
  }

  // altrimenti inserisco il mio gettone nella colonna
  grid_addr[column][pos] = grid.token;
  return 0;
}
