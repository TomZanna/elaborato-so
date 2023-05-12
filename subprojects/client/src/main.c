#include "libconnect4.h"
#include "msgq_utils.h"
#include "parse_args.h"
#include <stdio.h>

int main(const int argc, char *const argv[]) {
  struct args cmd_args = {0};

  EXIT_ON_ERR(parse_args(&cmd_args, argc, argv));

  struct msgq_config config;

  fputs("Connessione in corso... ", stdout);
  fflush(stdout);
  EXIT_ON_ERR(msgq_get_config(cmd_args.game_id, &config));
  fputs("Connesso!\n", stdout);

  return 0;
}
