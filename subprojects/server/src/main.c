#include "libconnect4.h"
#include "parse_args.h"
#include "shm_utils.h"
#include "sigint_utils.h"
#include <stdio.h>

int main(const int argc, char *const argv[]) {
  struct args cmd_args = {0};

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));
  printf("Impostazioni della partita:\n"
         "- Griglia di dimensione %ix%i\n"
         "- Timeout: %i secondi\n"
         "- Token P1: %c\n- Token P2: %c\n",
         cmd_args.grid_width, cmd_args.grid_height, cmd_args.timeout,
         cmd_args.token_player1, cmd_args.token_player2);

  printf("Inizializzazione in corso... ");
  fflush(stdout);
  sigint_attach_handler();

  int shm_id;

  EXIT_ON_ERR(shm_id =
                  shm_initialize(cmd_args.grid_height, cmd_args.grid_width));

  while (1) {
    char winner = shm_check_game();

    if (winner > 0 || shm_grid_full()) {
      if (winner == cmd_args.token_player1) {
        printf("vince 1\n");
      } else if (winner == cmd_args.token_player1) {
        printf("vince 2\n");
      } else {
        printf("pareggio\n");
      }

      // inizializzo la griglia di gioco
      shm_reset_board();
    }
  }

  return 0;
}
