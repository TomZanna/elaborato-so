#include "msgq_utils.h"
#include "libconnect4.h"
#include "parse_args.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int msgq_id = -1;
int players_pid[2] = {-1};

void msgq_handle_new_feedback(int sig);

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

  signal(MSGQ_FEEDBACK_SIGNAL, msgq_handle_new_feedback);

  for (int i = 0; i < 2; i++) {
    game_config.player_number = i;
    game_config.player_token =
        i == 0 ? args->token_player1 : args->token_player2;

    if (msgsnd(msgq_id, &game_config, MSGQ_STRUCT_SIZE(game_config), 0)) {
      perror("Errore nell'inizializzazione del campo di gioco");
      return -1;
    }
  }

  return 0;
}

int msgq_send_win(int player_number) {
  static struct msgq_status tmp_msg = {0};

  for (int i = 0; i < 2; i++) {
    tmp_msg.mtype = players_pid[i];

    if (i == player_number)
      tmp_msg.result = WIN;
    else
      tmp_msg.result = LOSE;

    if (msgsnd(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), 0)) {
      perror("Errore di comunicazione");
      return -1;
    }

    kill(tmp_msg.mtype, MSGQ_STATUS_SIGNAL);
  }

  return 0;
}

int msgq_send_tie() {
  static struct msgq_status tmp_msg = {0};

  tmp_msg.result = TIE; // tie

  for (int i = 0; i < 2; i++) {
    tmp_msg.mtype = players_pid[i];

    if (msgsnd(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), 0)) {
      perror("Errore di comunicazione");
      return -1;
    }

    kill(tmp_msg.mtype, MSGQ_STATUS_SIGNAL);
  }

  return 0;
}

void msgq_send_exit_status(void) {
  for (int i = 0; i < 2; i++) {
    if (players_pid[i] != -1) {
      kill(players_pid[i], SERVER_CLOSED_SIGNAL);
    }
  }
}

void msgq_handle_new_feedback(int sig) {
  struct msgq_feedback tmp_msg;

  signal(sig, msgq_handle_new_feedback);

  if (msgrcv(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), MSGQ_FEEDBACK_MTYPE,
             IPC_NOWAIT) == -1) {
    perror("Errore di comunicazione");
    EXIT_ON_ERR(-1);
  };

  if (tmp_msg.status == HELO) {
    players_pid[tmp_msg.client_num] = tmp_msg.client_pid;
  } else if (tmp_msg.status == LEAVING) {
    players_pid[tmp_msg.client_num] = -1;
    exit(0);
  }
}

void msgq_attach_handler(void) {
  signal(MSGQ_FEEDBACK_SIGNAL, msgq_handle_new_feedback);
}

void wait_random_bot(void) { wait(NULL); }

void no_more_random_bot(__attribute__((unused)) int sig) {
  fprintf(
      stderr,
      "Partita già al completo, non è possibile avviare il bot avversario\n");

  msgq_send_exit_status();
  exit(EXIT_FAILURE);
}

void start_random_client(int sig) {
  signal(sig, no_more_random_bot);

  int pid = fork();

  if (pid == 0) {
    char buf[5];
    snprintf(buf, sizeof(buf), "%i", msgq_id);

    close(1);
    execlp("connect4-client", "argv0", buf, "random", NULL);

    // kill al server
  } else if (pid > 0) {
    atexit(wait_random_bot);
  } else {
    perror("Errore durante l'avvio del bot avversario");
    EXIT_ON_ERR(-1);
  }
}
