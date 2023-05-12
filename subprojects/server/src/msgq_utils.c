#include "msgq_utils.h"
#include "libconnect4.h"
#include "parse_args.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

int msgq_id = -1;

void msgq_teardown(void) {
  if (msgq_id != -1)
    msgctl(msgq_id, IPC_RMID, NULL);
}

int msgq_initialize(void) {
  // registro callback da eseguire durante la terminazione del programma
  if (atexit(msgq_teardown) < 0) {
    // se non riesco a registrate la funzione, fermo tutto
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  msgq_id = msgget(IPC_PRIVATE, S_IRWXU | S_IRWXG | S_IRWXO);

  if (msgq_id == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco: ");
    return -1;
  }

  return msgq_id;
}

int msgq_send_config(const struct args *const args, const int shm_id,
                     const int sem_id) {
  struct msgq_config game_config = {
      .mtype = MSGQ_CONFIG_MTYPE,
      .grid_height = args->grid_height,
      .grid_width = args->grid_width,
      .timeout = args->timeout,
      .server_pid = getpid(),
      .sem_id = sem_id,
      .shm_id = shm_id,
  };

  for (int i = 0; i < 2; i++) {
    game_config.player_number = i;
    game_config.player_token =
        i == 0 ? args->token_player1 : args->token_player2;

    if (msgsnd(msgq_id, &game_config, MSGQ_CONFIG_SIZE, 0)) {
      perror("Errore nell'inizializzazione del campo di gioco:");
      return -1;
    }
  }

  return 0;
}
