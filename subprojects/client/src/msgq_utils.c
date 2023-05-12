#include "libconnect4.h"
#include <errno.h>
#include <stdio.h>
#include <sys/msg.h>

int msgq_id = -1;

int msgq_get_config(const int game_id, struct msgq_config *const config) {
  msgq_id = game_id;

  int ret =
      msgrcv(msgq_id, config, MSGQ_CONFIG_SIZE, MSGQ_CONFIG_MTYPE, IPC_NOWAIT);

  if (ret == -1) {
    switch (errno) {
    case EINVAL:
      fputs("ID di gioco non valido!\n", stdout);
      break;
    case ENOMSG:
      fputs("La partita è già al completo.\n", stdout);
      break;
    default:
      perror("Errore durante la connessione");
      break;
    }
    return -1;
  }

  return 0;
}
