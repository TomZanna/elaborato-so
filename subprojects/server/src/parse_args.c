#include "parse_args.h"
#include "libconnect4.h"
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINIMUM_GRID_WIDTH 5
#define MINIMUM_GRID_HEIGHT 5

#define HELP_OUTPUT_TEXT                                                       \
  "forza4_server [-t TIMEOUT] GRID_HEIGHT GRID_WIDTH TOKEN_P1 TOKEN_P2\n"      \
  "\nParametri posizionali e obbligatori:\n"                                   \
  "  GRID_HEIGHT    numero di righe della griglia di gioco\n"                  \
  "  GRID_WIDTH     numero di colonne della griglia di gioco\n"                \
  "  TOKEN_P1       gettone utilizzato dal giocatore 1\n"                      \
  "  TOKEN_P2       gettone utilizzato dal giocatore 2\n"                      \
  "\nParametri opzionali:\n"                                                   \
  "  -t TIMEOUT     secondi entro cui deve avvenire la mossa del giocatore.\n" \
  "                 Se impostato a 0 non ci sarà timeout (default = 0)\n"

int parse_args(struct args *const output, const int argc, char *const argv[]) {
  // parse degli args opzionali
  int c;
  while ((c = getopt(argc, argv, "ht:")) != -1)
    switch (c) {
    case 'h':
      printf(HELP_OUTPUT_TEXT);
      return -2;
    case 't':
      if (parse_uint(&output->timeout, optarg) != 0) {
        fprintf(stderr, "TIMEOUT deve essere un intero positivo\n");
        return -1;
      }
      break;
    default:
      return -1;
    }

  // parse degli args obbligatori
  if ((argc - optind) != 4) {
    fprintf(stderr, "Uso errato: lanciami con -h per ulteriori informazioni\n");
    return -1;
  }

  if (parse_uint(&output->grid_height, argv[optind]) != 0 ||
      output->grid_height < MINIMUM_GRID_HEIGHT) {
    fprintf(
        stderr,
        "GRID_HEIGHT deve essere un intero positivo maggiore o uguale a 5\n");
    return -1;
  }
  optind++;

  if (parse_uint(&output->grid_width, argv[optind]) != 0 ||
      output->grid_width < MINIMUM_GRID_WIDTH) {
    fprintf(
        stderr,
        "GRID_WIDTH deve essere un intero positivo maggiore o uguale a 5\n");
    return -1;
  }
  optind++;

  output->token_player1 = argv[optind][0];
  if (strlen(argv[optind]) != 1 || !isprint(output->token_player1)) {
    fprintf(stderr, "TOKEN_P1 deve essere un unico carattere stampabile\n");
    return -1;
  }
  optind++;

  output->token_player2 = argv[optind][0];
  if (strlen(argv[optind]) != 1 || !isprint(output->token_player2)) {
    fprintf(stderr, "TOKEN_P2 deve essere un unico carattere stampabile\n");
    return -1;
  }

  return 0;
}
