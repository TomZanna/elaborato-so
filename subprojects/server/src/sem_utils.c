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
    perror("Errore durante l'inizializzazione del campo di gioco");
    return -1;
  }

  // 2 per la connessione + 2 a testa per sincronizzare turno/mossa
  sem_id = semget(IPC_PRIVATE, 6, S_IRWXU | S_IRWXG | S_IRWXO);

  if (sem_id == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco");
    return -1;
  }

  return sem_id;
}

int sem_wait_ready(void) {
  // aspetto entrambi i giocatori
  if (sem_lib_wait(sem_id, 0, -2) == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco");
    return -1;
  }

  return 0;
}

int sem_signal_start(void) {
  // sblocco i giocatori in attesa
  if (sem_lib_signal(sem_id, 1, 2) == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco");
    return -1;
  }

  return 0;
}

int sem_signal_turn(int player_number) {
  // sblocco il giocatore affichè possa fare la sua mossa
  if (sem_lib_signal(sem_id, sem_getnum(player_number), 1) == -1) {
    perror("Errore durante l'arbitraggio della partita");
    return -1;
  }

  return 0;
}

int sem_wait_move(int player_number) {
  // attendo la giocata del giocatore
  if (sem_lib_wait(sem_id, sem_getnum(player_number) + 1, -1) == -1) {
    perror("Errore durante l'arbitraggio della partita");
    return -1;
  }

  return 0;
}
