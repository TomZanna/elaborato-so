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

int sem_signal_players(void) {
  struct sembuf sops;

  // il server si sincronizza per la connessione sul semaforo 0
  sops.sem_num = 0;
  sops.sem_op = 1;
  sops.sem_flg = 0;

  if (semop(sem_id, &sops, 1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}

int sem_wait_server(void) {
  struct sembuf sops;

  // il primo giocatore aspetta sul semaforo 1
  sops.sem_num = sem_getnum(player_number);
  sops.sem_op = -1;
  sops.sem_flg = 0;

  // se vengo risvegliato da un interrupt, torno in attesa
  int ret;
  do {
    ret = semop(sem_id, &sops, 1);
  } while (ret == -1 && errno == EINTR);

  if (ret == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}

int sem_signal_server(void) {
  struct sembuf sops;

  // il server aspetta il primo giocatore sul semaforo 1
  sops.sem_num = sem_getnum(player_number) + 1;
  sops.sem_op = 1;
  sops.sem_flg = 0;

  if (semop(sem_id, &sops, 1) == -1) {
    perror("Errore di sincronizzazione: ");
    return -1;
  }

  return 0;
}
