#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include "sem_utils.h"
#include "shm_utils.h"
#include "sigint_utils.h"
#include <stdio.h>

int main(const int argc, char *const argv[]) {
  // inizializzo gli handler per i segnali
  signal(SIGINT, sigint_handler);
  signal(SIGTERM, sigterm_handler);

  // analizzo gli argomenti ricevuti da riga di comando
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

  int msgq_id, sem_id, shm_id;

  EXIT_ON_ERR(msgq_id = msgq_initialize());

  EXIT_ON_ERR(sem_id = sem_initialize());
  EXIT_ON_ERR(shm_id =
                  shm_initialize(cmd_args.grid_height, cmd_args.grid_width));

  // pongo nella coda i messaggi di configurazione per i client
  EXIT_ON_ERR(msgq_send_config(&cmd_args, shm_id, sem_id));
  printf("Fatto!\nEcco l'ID di questa partita: %i \n", msgq_id);

  printf("In attesa che si connettano i due giocatori... ");
  fflush(stdout);
  EXIT_ON_ERR(sem_wait_ready());
  EXIT_ON_ERR(sem_signal_start());
  printf("possiamo cominciare!\n");

  int player_turn = 1; // inizia il secondo client (ultimo connesso)
  while (1) {
    // sblocco il giocatore
    EXIT_ON_ERR(sem_signal_turn(player_turn));

    // aspetto la mossa del giocatore
    EXIT_ON_ERR(sem_wait_move(player_turn));

    // controllo se c'è un nuovo vincitore
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
