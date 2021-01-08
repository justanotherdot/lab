#include <stdio.h>
#include <stdint.h>

int
main(void)
{
  int8_t x0 = 10;
  int8_t y0 = 6;

  uint8_t x1 = 10;
  uint8_t y1 = 6;

  printf("Addition via double subtraction\n");
  printf("signed: %d\n", x0 - (-y0));
  printf("unsigned: %d\n", x1 - (-y1));

  // $ clang -Wall -Werror add_via_sub.c -o dist/add_via_sub && ./add_via_sub
  // Addition via double subtraction
  // signed: 16
  // unsigned: 16
}
