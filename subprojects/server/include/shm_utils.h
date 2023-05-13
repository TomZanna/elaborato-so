int shm_initialize(const unsigned int grid_heidht,
                   const unsigned int grid_width);

char shm_check_game(void);

void shm_reset_board(void);

int shm_grid_full(void);

char _check_horizontal(int width, int height, char (*grid)[height]);
char _check_vertical(int width, int height, char (*grid)[height]);
char _check_diagonal(int width, int height, char (*grid)[height]);
char _check_back_diagonal(int width, int height, char (*grid)[height]);
