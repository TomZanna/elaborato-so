#include "sigint_utils.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_KEY_PRESS 2
#define SIGINT_TIMEOUT 30

void sigint_handler(int signum) {
  static int count = MAX_KEY_PRESS;

  sigint_attach_handler();

  if (signum == SIGALRM) {
    printf("Non hai premuto CTRL+C entro i %i secondi.\n", SIGINT_TIMEOUT);
    count = MAX_KEY_PRESS;

    return;
  }

  if (--count > 0) {
    alarm(SIGINT_TIMEOUT);

    printf("Premi CTRL+C entro %i secondi per altre %i volte per terminare la "
           "partita.\n",
           SIGINT_TIMEOUT, count);
  } else {
    printf("Hai premuto CTRL+C %i volte... la partita è stata terminata per "
           "tutti!\n",
           MAX_KEY_PRESS);

    exit(0);
  }
}

void sigint_attach_handler(void) {
  signal(SIGALRM, sigint_handler);
  signal(SIGINT, sigint_handler);
}
