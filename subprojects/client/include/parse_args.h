#ifndef CLIENT_STRUCT_ARGS_GUARD
#define CLIENT_STRUCT_ARGS_GUARD
struct args {
  unsigned int game_id;
  int single_player;
};
#endif

int parse_args(struct args *const, int, char *const *);
