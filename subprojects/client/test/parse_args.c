#include "parse_args.h"
#include <assert.h>

#define ARR_LENGHT(array) (sizeof array) / (sizeof array[0])
extern int optind; // index used by getopt

void help_option_provided(void) {
  optind = 1;
  char *const argv[] = {"client", "-h"};
  struct args output = {0};

  int ret_code = parse_args(&output, ARR_LENGHT(argv), argv);

  assert(ret_code == 2); // help message printed, should return 2
}

void no_optional_argument_provided(void) {
  optind = 1;
  char *const argv[] = {"client", "123", "user"};
  struct args output = {0};

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == 0);

  assert(output.game_id == 123);
  assert(output.single_player == 0);
}

void single_player_argument_provided(void) {
  optind = 1;
  char *const argv[] = {"client", "-s", "123", "user"};
  struct args output = {0};

  assert(parse_args(&output, ARR_LENGHT(argv), argv) == 0);

  assert(output.game_id == 123);
  assert(output.single_player == 1);
}

void invalid_game_id(void) {
  optind = 1;
  char *const argv[] = {"client", "-s", "1a23", "user"};
  struct args output = {0};

  assert(parse_args(&output, ARR_LENGHT(argv), argv) != 0);
}

void missing_username(void) {
  optind = 1;
  char *const argv[] = {"client", "-s", "123"};
  struct args output = {0};

  assert(parse_args(&output, ARR_LENGHT(argv), argv) != 0);
}

int main(void) {
  help_option_provided();
  no_optional_argument_provided();
  single_player_argument_provided();

  invalid_game_id();
  missing_username();

  return 0;
}
