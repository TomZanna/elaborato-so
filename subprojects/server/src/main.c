#include "libconnect4.h"
#include "parse_args.h"
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

  return 0;
}
