#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include "sem_utils.h"
#include "shm_utils.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int random_mode = 0;

void sigalrm_handler(__attribute__((unused)) int signal) {
  printf("Tempo scaduto! Il turno passa all'avversario.\n");
}

void game_ended_handler(__attribute__((unused)) int sig) {
  printf("\nIl server ha ordinato la chiusura della partita\n");
  exit(EXIT_SUCCESS);
}

int main(const int argc, char *const argv[]) {
  srand(time(NULL));
  signal(GAME_ENDED_SIGNAL, game_ended_handler);

  struct args cmd_args = {0};

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));

  struct msgq_config config;

  printf("Connessione in corso... ");
  fflush(stdout);

  // leggo la configurazione della partita
  EXIT_ON_ERR(msgq_get_config(cmd_args.game_id, &config));
  // configuro i semafori
  sem_config(config.sem_id, config.player_number);
  // mi collego alla memoria condivisa
  EXIT_ON_ERR(shm_attach(config.shm_id, config.grid_width, config.grid_height,
                         config.player_token));

  // se mio padre è il server, gioco in modalità casuale
  if (config.server_pid == getppid())
    random_mode = 1;
  else if (cmd_args.single_player)
    kill(config.server_pid, RANDOM_BOT_SIGNAL);

  printf("Connesso!\nIn attesa dell'avversario... ");
  fflush(stdout);

  EXIT_ON_ERR(sem_signal_ready());
  EXIT_ON_ERR(sem_wait_start());

  printf("eccolo! Che la sfida abbia inizio\n");

  while (1) {
    EXIT_ON_ERR(sem_wait_turn());

    if (random_mode) {
      int output;
      do {
        output = (rand() % config.grid_width) + 1;
      } while (shm_input_valid(output - 1) != 0);
    } else {
      printf("\033[1;1H\033[2J"); // clear screen

      if (config.timeout) {
        signal(SIGALRM, sigalrm_handler);
        alarm(config.timeout);
      }

      unsigned int output;
      while (1) {
        char buffer[5];
        shm_print_grid();

        printf("Digitare il numero della colonna in cui inserire il gettone: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
          if (errno == EINTR)
            break;
          else {
            perror("Errore durante l'acquisizione");
            EXIT_ON_ERR(-1);
          }
        }

        printf("\033[1;1H\033[2J"); // clear screen

        buffer[strcspn(buffer, "\n")] = 0;
        if (parse_uint(&output, buffer) == -1 || output < 1 ||
            output > config.grid_width) {
          printf("Input non valido!\n");
        } else if (shm_input_valid(output - 1) == -1) {
          printf("La colonna selezionata è piena\n");
        } else
          break;
      }

      if (config.timeout)
        alarm(0);

      shm_print_grid();
    }

    EXIT_ON_ERR(sem_signal_move());
  }

  return 0;
}
