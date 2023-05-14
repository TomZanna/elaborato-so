#include "msgq_utils.h"
#include "libconnect4.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/msg.h>
#include <unistd.h>

int msgq_id = -1;
int player_num = -1;
int server_pid = -1;

int msgq_get_config(const int game_id, struct msgq_config *const config) {
  msgq_id = game_id;

  int ret = msgrcv(msgq_id, config, MSGQ_STRUCT_SIZE(*config),
                   MSGQ_CONFIG_MTYPE, IPC_NOWAIT);

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

  server_pid = config->server_pid;
  player_num = config->player_number;

  signal(SIGINT, msgq_send_leaving);
  signal(MSGQ_FEEDBACK_SIGNAL, msgq_handle_new_status);

  struct msgq_feedback feedback;
  feedback.mtype = MSGQ_FEEDBACK_MTYPE;
  feedback.status = HELO;
  feedback.client_num = config->player_number;
  feedback.client_pid = getpid();

  if (msgsnd(msgq_id, &feedback, MSGQ_STRUCT_SIZE(feedback), IPC_NOWAIT) ==
      -1) {
    perror("Errore durante la connessione");
    return -1;
  }

  kill(config->server_pid, MSGQ_FEEDBACK_SIGNAL);

  return 0;
}

void msgq_handle_new_status(int sig) {
  static int lose = 0, win = 0;

  struct msgq_status tmp_msg;

  signal(sig, msgq_handle_new_status);

  if (msgrcv(msgq_id, &tmp_msg, MSGQ_STRUCT_SIZE(tmp_msg), getpid(),
             IPC_NOWAIT) == -1) {
    perror("Errore di comunicazione");
    EXIT_ON_ERR(-1);
  }

  printf("\033[1;1H\033[2J"); // clear screen
  switch (tmp_msg.result) {
  case TIE:
    printf("Pareggio =|");
    break;
  case WIN:
    win++;
    printf("Vittoria! =)"
           "Il punteggio è %i-%i",
           win, lose);
    break;
  case LOSE:
    lose++;
    printf("Sconfitta! =(\n"
           "Il punteggio è %i-%i",
           win, lose);
    break;
  case ABANDONED:
    printf(
        "L'avversario ha abbandonato la partita. Hai vinto a tavolino! *)\n");
    exit(EXIT_SUCCESS);

  default:
    break;
  }

  printf("\nPremi ENTER per iniziare la prossima partita");
  while (getchar() != '\n' && !random_mode)
    ;
  printf("La partita inizierà a breve!\n");
}

void msgq_send_leaving(int sig) {
  struct msgq_feedback feedback;
  feedback.mtype = MSGQ_FEEDBACK_MTYPE;
  feedback.status = LEAVING;
  feedback.client_num = player_num;
  feedback.client_pid = getpid();

  if (msgsnd(msgq_id, &feedback, MSGQ_STRUCT_SIZE(feedback), 0) == -1) {
    perror("Errore durante la chiusura");
    EXIT_ON_ERR(-1);
  }

  kill(server_pid, MSGQ_FEEDBACK_SIGNAL);

  exit(0);
}
