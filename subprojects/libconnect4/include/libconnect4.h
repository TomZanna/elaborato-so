#include <stdlib.h>

#define EXIT_ON_ERR(func)                                                      \
  if ((func) < 0)                                                              \
    exit(EXIT_FAILURE);

struct msgq_config {
  int mtype;
  int shm_id;
  int sem_id;
  int server_pid;
  char player_token;
  int player_number;
  int timeout;
  int grid_width;
  int grid_height;
};

int parse_uint(unsigned int *const, const char *const);
