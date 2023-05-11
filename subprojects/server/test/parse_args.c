#include "parse_args.h"
#include <assert.h>

#define ARR_LENGHT(array) (sizeof array) / (sizeof array[0])
extern int optind; // index used by getopt

void no_optional_argument_provided(void) {
  optind = 1;
  char *const argv[] = {"server", "6", "7", "X", "O"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == 0);

  assert(output.timeout == 0);
  assert(output.grid_height == 6);
  assert(output.grid_width == 7);
  assert(output.token_player1 == 'X');
  assert(output.token_player2 == 'O');
}

void timeout_option_provided(void) {
  optind = 1;
  char *const argv[] = {"server", "-t", "10", "6", "7", "X", "O"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == 0);

  assert(output.timeout == 10);
  assert(output.grid_height == 6);
  assert(output.grid_width == 7);
  assert(output.token_player1 == 'X');
  assert(output.token_player2 == 'O');
}

void help_option_provided(void) {
  optind = 1;
  char *const argv[] = {"server", "-h"};
  struct args output;

  int ret_code = parse_args(&output, ARR_LENGHT(argv), argv);

  assert(ret_code == -2); // help message printed, should return -2
}

void invalid_grid_height(void) {
  optind = 1;
  char *const argv[] = {"server", "4", "5", "X", "O"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == -1);
}

void invalid_grid_width(void) {
  optind = 1;
  char *const argv[] = {"server", "5", "4", "X", "O"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == -1);
}

void invalid_player1_token(void) {
  optind = 1;
  char *const argv[] = {"server", "5", "5", "\n", "O"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == -1);
}

void invalid_player2_token(void) {
  optind = 1;
  char *const argv[] = {"server", "5", "5", "X", "\t"};
  struct args output;

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == -1);
}

int main(void) {
  no_optional_argument_provided();
  timeout_option_provided();
  help_option_provided();

  invalid_grid_height();
  invalid_grid_width();
  invalid_player1_token();
  invalid_player2_token();

  return 0;
}
