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
int players_count = 0;
int players_pid[2] = {-1};

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

  atexit(msgq_send_exit_status);
  signal(MSGQ_FEEDBACK_SIGNAL, msgq_handle_new_feedback);
  signal(RANDOM_CLIENT_SIGNAL, start_random_client);

  // inserisco nella coda due messaggi con la conf della partita
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

int msgq_send_abandoned(int player_number) {
  struct msgq_status tmp_msg = {
      .mtype = players_pid[player_number],
      .result = ABANDONED,
  };

  // se c'è l'avversario, lo informo che ha vinto a tavolino
  if (tmp_msg.mtype != -1) {
    if (msgsnd(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), 0)) {
      perror("Errore di comunicazione");
      return -1;
    }

    kill(tmp_msg.mtype, MSGQ_STATUS_SIGNAL);
  }

  return 0;
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
    // salvo il pid del giocatore che si è connesso
    players_count++;
    players_pid[tmp_msg.client_num] = tmp_msg.client_pid;
  } else if (tmp_msg.status == LEAVING) {
    // informo il client avversario che ha vinto a tavolino
    msgq_send_abandoned((tmp_msg.client_num + 1) % 2);

    sleep(2);
    exit(0);
  }
}

void wait_random_bot(void) { wait(NULL); }

void start_random_client(__attribute_maybe_unused__ int sig) {

  // il secondo client chiede il bot, errore
  if (players_count >= 2) {
    fprintf(stderr, "Sono ammessi solo due giocatori\n");
    exit(EXIT_FAILURE);
  }

  int pid = fork();

  if (pid == 0) {
    char buf[100];
    snprintf(buf, sizeof(buf), "%i", msgq_id);

    close(1);
    execlp("./connect4-client", "connect4-client", buf, "random", NULL);
    perror("Errore durante l'avvio del bot avversario");

    // errore, termino la partita per tutti
    kill(SIGTERM, getppid());
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    atexit(wait_random_bot);
  } else {
    perror("Errore durante l'avvio del bot avversario");
    EXIT_ON_ERR(-1);
  }
}
