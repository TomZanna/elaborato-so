#include "libconnect4.h"
#include "parse_args.h"
#include <stdio.h>

int main(const int argc, char *const argv[]) {
  struct args cmd_args;

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));

  printf("GAME_ID: %i\t SINGLE_PLAYER: %i\n", cmd_args.game_id,
         cmd_args.single_player);
  return 0;
}
