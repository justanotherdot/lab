#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include "utf8_parser.h"

// 0xxx xxxx    A single-byte US-ASCII code (from the first 127 characters)
// 110x xxxx    One more byte follows
// 1110 xxxx    Two more bytes follow
// 1111 0xxx    Three more bytes follow
// 10xx xxxx    A continuation of one of the multi-byte characters

// 0xC0 -> 12*16^1 + 0 -> 192 -> 1100 0000
// 0xC1 -> 192 + 1 = 193      -> 1100 0001

int main(int argc, char** argv)
{
  return run_utf8_parser(argc, argv);
}

// driver.
int run_utf8_parser(int argc, char** argv) {
  if (argc != 2) {
    printf("Please provide a UTF encoded string to decode\n");
    return EXIT_FAILURE;
  }

  char* parse_target = argv[1];
  size_t parse_target_len = strlen(parse_target);
  printf("You've asked me to analyse the string '%s'\n", parse_target);

  // Set the locale to ensure character encodings output the right values.
  char* l = setlocale(LC_ALL, "");
  if (l == NULL) {
    printf("Locale not set\n");
  } else {
    printf("Local set to %s\n", l);
  }

  size_t i;
  for (i = 0; i < parse_target_len; ++i) {
    print_byte(parse_target[i]);
    printf("byte %ld: %c\n", (long)i, parse_target[i]);
  }

  return EXIT_SUCCESS;
}

// utilities.

// XXX This is not portable!
// We're assuming a byte is 8-bits, but this could be made variable.
// Per machine architectures that don't support a byte as 8-bits.
void print_byte(char c)
{
  int i;
  for (i = 7; i >= 0; i--) {
    int mask = (c>>i) & 1;
    printf("%d", mask);

    if (i == 4)
      printf(" ");
  }
  printf(" ");
}

// is_printable?
// parse_code_point

/*UTF8ParseResult parse_next_code_point(char* base)*/
/*{*/
  /*UTF8ParseResult rv;*/
  /*UTF8Parse parse_result;*/



  /*return rv;*/
/*}*/

/*UTF8ParseResult*/
/*utf8_parse_result_result(UTF8ParseResult parse_result)*/
/*{*/
  /*UTF8ParseResult rv;*/
  /*rv.tag = Ok;*/
  /*rv.result = parse_result;*/

/*}*/

/*UTF8ParseResult*/
/*utf8_parse_result_err(char* str)*/
/*{*/
/*}*/
