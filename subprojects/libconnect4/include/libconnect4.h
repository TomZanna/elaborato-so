#define _POSIX_SOURCE
#include <signal.h>
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

struct msgq_status {
  long mtype;
  enum match_result { TIE, LOSE, WIN, ABANDONED } result;
};

struct msgq_feedback {
  long mtype;
  int client_pid;
  int client_num;
  enum client_status { HELO, LEAVING } status;
};

#endif

#define MSGQ_CONFIG_MTYPE 1
#define MSGQ_FEEDBACK_MTYPE 2
#define MSGQ_STRUCT_SIZE(in_struct)                                            \
  sizeof(in_struct) - sizeof((in_struct).mtype)

#define MSGQ_FEEDBACK_SIGNAL SIGUSR1
#define MSGQ_STATUS_SIGNAL SIGUSR1

#define SERVER_CLOSED_SIGNAL SIGUSR2
#define RANDOM_CLIENT_SIGNAL SIGUSR2

int parse_uint(unsigned int *const, const char *const);

int sem_getnum(int player_num);

int sem_lib_wait(int sem_id, int sem_num, int sem_op);

int sem_lib_signal(int sem_id, int sem_num, int sem_op);
