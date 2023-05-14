#include "parse_args.h"
#include "libconnect4.h"
#include <getopt.h>
#include <stdio.h>

#define HELP_OUTPUT_TEXT                                                       \
  "forza4_server [-s] GAME_ID USERNAME\n"                                      \
  "\nParametri posizionali e obbligatori:\n"                                   \
  "  GAME_ID       id della partita a cui connetersi\n"                        \
  "  USERNAME     nome con cui sarai identificato durante la partita\n"        \
  "\nParametri opzionali:\n"                                                   \
  "  -s           flag per abilitare la modalità single player.\n"

int parse_args(struct args *const output, const int argc, char *const argv[]) {
  // parse degli args opzionali
  int c;
  while ((c = getopt(argc, argv, "hs")) != -1)
    switch (c) {
    case 'h':
      printf(HELP_OUTPUT_TEXT);
      return -2;
    case 's':
      output->single_player = 1; // gamemode
      break;
    default:
      return -1;
    }

  // parse degli args obbligatori
  if ((argc - optind) != 2) {
    fputs("Uso errato: lanciami con -h per ulteriori informazioni\n", stderr);
    return -1;
  }

  /* GAME_ID è l'id della coda di messaggi generato dalla syscall msgget.
   * Pertanto possiamo assumere che debba essere positivo,
   * altrimenti il server avrebbe già terminato restituendo un errore.
   */
  if (parse_uint(&output->game_id, argv[optind]) != 0) {
    fputs("GAME_ID deve essere un intero maggiore di 0\n", stderr);
    return -1;
  }

  return 0;
}
