struct args {
  unsigned int game_id;
  int single_player;
};

int parse_args(struct args *const, int, char *const *);
