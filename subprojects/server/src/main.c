#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include "sem_utils.h"
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

  int msgq_id, sem_id, shm_id;

  EXIT_ON_ERR(msgq_id = msgq_initialize());

  EXIT_ON_ERR(sem_id = sem_initialize());
  EXIT_ON_ERR(shm_id =
                  shm_initialize(cmd_args.grid_height, cmd_args.grid_width));

  EXIT_ON_ERR(msgq_send_config(&cmd_args, shm_id, sem_id));
  printf("Fatto!\nEcco l'ID di questa partita: %i \n", msgq_id);

  msgq_attach_handler();

  printf("In attesa che si connettano i due giocatori... ");
  fflush(stdout);
  EXIT_ON_ERR(sem_wait_ready());
  EXIT_ON_ERR(sem_signal_start());
  printf("possiamo cominciare!\n");

  int player_turn = 1; // inizia il secondo (ultimo connesso)
  while (1) {
    // sblocco uno dei due giocatori
    EXIT_ON_ERR(sem_signal_turn(player_turn));

    // aspetto la mossa del giocatore
    EXIT_ON_ERR(sem_wait_move(player_turn));

    // controllo se uno dei due ha vinto
    char winner = shm_check_game();

    if (winner > 0 || shm_grid_full()) {
      if (winner == cmd_args.token_player1) {
        msgq_send_win(0);
      } else if (winner == cmd_args.token_player2) {
        msgq_send_win(1);
      } else {
        msgq_send_tie();
      }

      // inizializzo la griglia di gioco
      shm_reset_board();
    }

    // ora tocca all'altro giocatore
    player_turn = (player_turn + 1) % 2;
  }

  return 0;
}
