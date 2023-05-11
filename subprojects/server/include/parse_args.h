struct args {
  unsigned int grid_height;
  unsigned int grid_width;
  unsigned int timeout;
  char token_player1;
  char token_player2;
  char _gap0[2];
};

int parse_args(struct args *const, int, char *const *);
