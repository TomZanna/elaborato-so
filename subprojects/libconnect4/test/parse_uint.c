#include "libconnect4.h"
#include <assert.h>

void positive_integer_input(void) {
  unsigned int output = 0;

  assert(parse_uint(&output, "12345") == 0);
  assert(output == 12345);
}

void with_leading_zero_input(void) {
  unsigned int output = 0;

  assert(parse_uint(&output, "00789") == 0);
  // no leading zero because it's not base 8
  assert(output == 789);
}

void out_of_range_input(void) {
  unsigned int output = 0;

  assert(parse_uint(&output, "4294967296") != 0);
}

void negative_input(void) {
  unsigned int output = 0;

  assert(parse_uint(&output, "-123") != 0);
}

void non_digit_input(void) {
  unsigned int output = 0;

  assert(parse_uint(&output, "12c45") != 0);
}

int main(void) {
  positive_integer_input();
  with_leading_zero_input();

  out_of_range_input();
  negative_input();
  non_digit_input();

  return 0;
}
