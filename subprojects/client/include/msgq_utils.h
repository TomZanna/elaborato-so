#include "libconnect4.h"

extern int random_mode;

int msgq_get_config(const int game_id, struct msgq_config *const config);

void msgq_attach_handler(void);
