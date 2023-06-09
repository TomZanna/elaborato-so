#include "parse_args.h"

int msgq_send_config(const struct args *const args, const int shm_id,
                     const int sem_id);

int msgq_initialize(void);

int msgq_send_win(int player_number);

int msgq_send_tie();

void msgq_handle_new_feedback(int sig);

void msgq_send_game_ended(void);

void handle_bot_request(int sig);
