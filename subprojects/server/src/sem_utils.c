#include "sem_utils.h"
#include "libconnect4.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>

int sem_id = -1;

void sem_teardown(void) {
  if (sem_id != -1)
    semctl(sem_id, 0, IPC_RMID);
}

int sem_initialize(void) {
  // registro callback da eseguire durante la terminazione del programma
  if (atexit(sem_teardown) < 0) {
    // se non riesco a registrate la funzione, fermo tutto
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  // 1 per la connessione + 2 a testa per sincronizzare turno/mossa
  sem_id = semget(IPC_PRIVATE, 5, S_IRWXU | S_IRWXG | S_IRWXO);

  if (sem_id == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return sem_id;
}

int sem_wait_players(void) {
  struct sembuf sops;
  sops.sem_num = 0;
  sops.sem_op = -2; // aspetto entrambi i giocatori
  sops.sem_flg = 0;

  // se vengo risvegliato da un interrupt, torno in attesa
  int ret;
  do {
    ret = semop(sem_id, &sops, 1);
  } while (ret == -1 && errno == EINTR);

  if (ret == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return 0;
}

int sem_signal_player(int player_number) {
  struct sembuf sops;

  // il primo giocatore si sincronizza sul semaforo 1
  sops.sem_num = sem_getnum(player_number);
  sops.sem_op = 1;
  sops.sem_flg = 0;

  if (semop(sem_id, &sops, 1) == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return 0;
}

int sem_wait_player(int player_number) {
  struct sembuf sops;

  // il primo giocatore mi sbloccherà tramite il semaforo 2
  sops.sem_num = sem_getnum(player_number) + 1;
  sops.sem_op = -1;
  sops.sem_flg = 0;

  // se vengo risvegliato da un interrupt, torno in attesa
  int ret;
  do {
    ret = semop(sem_id, &sops, 1);
  } while (ret == -1 && errno == EINTR);

  if (ret == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return 0;
}
