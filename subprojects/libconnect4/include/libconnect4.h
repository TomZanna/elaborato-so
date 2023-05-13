#include <stdlib.h>

#define EXIT_ON_ERR(func)                                                      \
  if ((func) < 0)                                                              \
    exit(EXIT_FAILURE);

#ifndef LIBCONNECT4_STRUCTS_GUARD
#define LIBCONNECT4_STRUCTS_GUARD
struct msgq_config {
  long mtype;
  int shm_id;
  int sem_id;
  int server_pid;
  char player_token;
  int player_number;
  unsigned int timeout;
  unsigned int grid_width;
  unsigned int grid_height;
};
#endif

#define MSGQ_CONFIG_MTYPE 1
#define MSGQ_CONFIG_SIZE sizeof(struct msgq_config) - sizeof(long)

int parse_uint(unsigned int *const, const char *const);

int sem_getnum(int player_num);

int sem_lib_wait(int sem_id, int sem_num, int sem_op);

int sem_lib_signal(int sem_id, int sem_num, int sem_op);
