#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include "sem_utils.h"
#include "shm_utils.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void sigalrm_handler(int signal) {
  fputs("Tempo scaduto! Il turno passa all'avversario.\n", stdout);
}

int main(const int argc, char *const argv[]) {
  struct args cmd_args = {0};

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));

  struct msgq_config config;

  fputs("Connessione in corso... ", stdout);
  fflush(stdout);
  EXIT_ON_ERR(msgq_get_config(cmd_args.game_id, &config));
  sem_config(config.sem_id, config.player_number);
  EXIT_ON_ERR(shm_attach(config.shm_id, config.grid_width, config.grid_height,
                         config.player_token));

  fputs("Connesso!\nIn attesa dell'avversario... ", stdout);
  fflush(stdout);

  sem_signal_players();
  EXIT_ON_ERR(sem_wait_server());

  fputs("eccolo! Che la sfida abbia inizio\n", stdout);

  while (1) {
    printf("\e[1;1H\e[2J"); // clear screen
    if (config.timeout) {
      signal(SIGALRM, sigalrm_handler);
      alarm(config.timeout);
    }

    unsigned int output;
    while (1) {
      char buffer[5];
      shm_print_grid();

      fputs("Digitare il numero della colonna in cui inserire il gettone: ",
            stdout);

      if (fgets(buffer, sizeof(buffer), stdin) == NULL && errno == EINTR) {
        break;
      }

      printf("\e[1;1H\e[2J"); // clear screen

      buffer[strcspn(buffer, "\n")] = 0;
      if (parse_uint(&output, buffer) == -1 || output > config.grid_width) {
        fputs("Input non valido!\n", stdout);
      } else if (shm_input_valid(output - 1) == -1) {
        fputs("La colonna selezionata è piena\n", stdout);
      } else
        break;
    }

    if (config.timeout)
      alarm(0);

    shm_print_grid();

    EXIT_ON_ERR(sem_signal_server());

    EXIT_ON_ERR(sem_wait_server());
  }

  return 0;
}
