#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include "sem_utils.h"
#include <stdio.h>

int main(const int argc, char *const argv[]) {
  struct args cmd_args = {0};

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));

  struct msgq_config config;

  fputs("Connessione in corso... ", stdout);
  fflush(stdout);
  EXIT_ON_ERR(msgq_get_config(cmd_args.game_id, &config));
  sem_config(config.sem_id, config.player_number);

  fputs("Connesso!\nIn attesa dell'avversario... ", stdout);
  fflush(stdout);

  sem_signal_players();
  EXIT_ON_ERR(sem_wait_server());

  fputs("eccolo! Che la sfida abbia inizio\n", stdout);

  EXIT_ON_ERR(sem_signal_server());

  return 0;
}
