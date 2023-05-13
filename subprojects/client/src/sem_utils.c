#include "libconnect4.h"
#include <errno.h>
#include <stdio.h>
#include <sys/sem.h>

int sem_id = -1;
int player_number = -1;

void sem_config(int new_sem_id, int new_player_number) {
  sem_id = new_sem_id;
  player_number = new_player_number;

  return;
}

int sem_signal_ready(void) {
  if (sem_lib_signal(sem_id, 0, 1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}

int sem_wait_start(void) {
  if (sem_lib_wait(sem_id, 1, -1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}

int sem_wait_turn(void) {
  // il primo giocatore aspetta sul semaforo 1
  if (sem_lib_wait(sem_id, sem_getnum(player_number), -1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}

int sem_signal_move(void) {
  if (sem_lib_signal(sem_id, sem_getnum(player_number) + 1, 1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}
