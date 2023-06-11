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
int players_pid[2] = {-1, -1};

void msgq_teardown(void) {
  if (msgq_id != -1)
    msgctl(msgq_id, IPC_RMID, NULL);
}

int msgq_initialize(void) {
  // registro callback da eseguire durante la terminazione del programma
  if (atexit(msgq_teardown) < 0) {
    // se non riesco a registrate la funzione, fermo tutto
    perror("Errore durante l'inizializzazione del campo di gioco");
    return -1;
  }

  msgq_id = msgget(IPC_PRIVATE, S_IRWXU | S_IRWXG | S_IRWXO);

  if (msgq_id == -1) {
    perror("Errore durante l'inizializzazione del campo di gioco");
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

  // registro la funzione che comunica la fine della partita ai client
  atexit(msgq_send_game_ended);

  signal(MSGQ_FEEDBACK_SIGNAL, msgq_handle_new_feedback);
  signal(RANDOM_BOT_SIGNAL, handle_bot_request);

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

void msgq_send_game_ended(void) {
  for (int i = 0; i < 2; i++) {
    if (players_pid[i] != -1) {
      kill(players_pid[i], GAME_ENDED_SIGNAL);
    }
  }
}

int msgq_send_table_win(int player_number) {
  struct msgq_status tmp_msg = {
      .mtype = players_pid[player_number],
      .result = TABLE_WIN,
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

  // registro nuovamente la funzione
  signal(sig, msgq_handle_new_feedback);

  if (msgrcv(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), MSGQ_FEEDBACK_MTYPE,
             IPC_NOWAIT) == -1) {
    perror("Errore di comunicazione");
    EXIT_ON_ERR(-1);
  };

  if (tmp_msg.status == HELO) {
    // salvo il pid del giocatore che si è connesso
    players_pid[tmp_msg.client_num] = tmp_msg.client_pid;
  } else if (tmp_msg.status == LEAVING) {
    // informo il client avversario che ha vinto a tavolino
    msgq_send_table_win((tmp_msg.client_num + 1) % 2);

    sleep(2);
    exit(0);
  }
}

void wait_random_bot(int sig) {
  signal(sig, wait_random_bot);
  wait(NULL);
}

void handle_bot_request(__attribute__((unused)) int sig) {

  int pid = fork();

  if (pid == 0) {
    char buf[100];
    snprintf(buf, sizeof(buf), "%i", msgq_id);

    close(1);
    execlp("./connect4-client", "connect4-client", buf, "random", NULL);
    perror("Errore durante l'avvio del bot avversario");
    fprintf(stderr, "Assicurati che il binario del client sia nella CWD\n");

    // errore, ordino al server di terminare la partita per tutti
    kill(getppid(), SIGTERM);
    _exit(EXIT_FAILURE);
  } else if (pid > 0) {
    signal(SIGCHLD, wait_random_bot);
  } else {
    perror("Errore durante l'avvio del bot avversario");
    EXIT_ON_ERR(-1);
  }
}
