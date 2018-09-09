#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <ctype.h>
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
    printf("Please provide a single UTF encoded string to decode\n");
    return EXIT_FAILURE;
  }

  char* parse_target = argv[1];
  size_t parse_target_len = strlen(parse_target);
  printf("You've asked me to analyse the string '%s'\n", parse_target);

  // Set the locale to ensure character encodings output the right values.
  maybe_print_locale_present();
  analyse_bytes_of_str(parse_target, parse_target_len);
  printf("\n");

  printf("Printing the given string in a UTF8 aware manner ... \n");
  print_utf8_str(parse_target, parse_target_len);

  return EXIT_SUCCESS;
}

// utilities.

//TODO void print_utf8_str_ln(char* str, size_t len)
void print_utf8_str(char* str, size_t len)
{
  char src[5];
  size_t i;
  for (i = 0; i < len; ++i) {
    if (is_ascii(str[i]) && isprint(str[i])) {
      printf("%c", str[i]);
    } else {
      int num_extra_bytes = num_bytes_follow(str[i]);
      // FIXME This could easily blow up if we have a byte that
      // says there is more to follow but not actually have anything else there!
      if (num_extra_bytes == 1) {
        src[0] = str[i];
        src[1] = str[i+2];
        src[2] = '\0';
      } else if (num_extra_bytes == 2) {
        // N.B. For posterity,
        // this was originally how I was trying to craft a wchar_t 'by hand'.
        /*wide_char = (unsigned char)str[i] << 16 | (unsigned char)str[i+1] << 8 | (unsigned char)str[i+2];*/
        src[0] = str[i];
        src[1] = str[i+1];
        src[2] = str[i+2];
        src[3] = '\0';
      } else if (num_extra_bytes == 3) {
        src[0] = str[i];
        src[1] = str[i+1];
        src[2] = str[i+2];
        src[3] = str[i+3];
        src[4] = '\0';
      } else {
        fprintf(stderr, "ERROR: Invalid character found at position %ld", (long)i);
        exit(1);
      }

      printf("%s", src);

      i+=num_extra_bytes;
    }
    /*} else if (is_cont(str[i])) {*/
      /*[>fprintf(stderr, "WARNING: Rogue unicode continuation foundn\n");<]*/
      /*[>fprintf(stderr, "         This means the string is probably mangled.\n");<]*/
    /*}*/
  }
}

// All unicode 'continuation' bytes will be 'signed'. This function is a
// utility for this module to do an unsigned (and therefore defind) left shift
// and then shift the leftmost bit back.
// TODO This could be generalised to left shift 'safely' for both signed and
// unsgined ints.
unsigned int shift_unicode_continuation(unsigned char n, size_t amt)
{
  unsigned int rv = 0;

  // clear bit.
  /*unsigned int n1 = n & ~(1UL << 7);*/
  unsigned int n1 = n;

  print_int(n);
  printf("\n");
  print_int(n1);

  rv |= n1 << amt;

  return rv;
}

// FIXME This currently pads the number of bytes by 3, but it could be more.
void analyse_bytes_of_str(char* str, size_t len)
{
  size_t i;
  for (i = 0; i < len; ++i) {
    // You can print larger 'wide ints' by doing
    // `printf("%lc", (wint_t)c);` but must ensure:
    //   1. LC_ALL is set (i.e. some locale is set)
    //   2. You are passing the large-enough integer
    //      that represents the glyph and not a
    //      truncated byte.
    print_byte(str[i]);
    printf(
      "byte %3ld: '%c', ascii? %5s, cont? %5s, num_bytes_follow: %1d\n",
      (long)i,
      str[i],
      is_ascii(str[i]) ? "true" : "false",
      is_cont(str[i]) ? "true" : "false",
      num_bytes_follow(str[i])
    );
  }
}

void maybe_print_locale_present()
{
  char* l = setlocale(LC_ALL, "");
  if (l == NULL) {
    printf("Locale not set\n");
  } else {
    printf("Locale set to %s\n", l);
  }
}

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

// XXX This is not portable!
// We're assuming a byte is 8-bits, but this could be made variable.
// Per machine architectures that don't support a byte as 8-bits.
void print_int(int c)
{
  int i;
  for (i = (8*4)-1; i >= 0; i--) {
    int mask = (c>>i) & 1;
    printf("%d", mask);

    if (i%4 == 0)
      printf(" ");
  }
  printf(" ");
}

char bit_at(size_t pos, int n)
{
  return (n>>(pos-1))&1;
}

bool bit_set_at(size_t pos, int n)
{
  return bit_at(pos, n) == 1;
}

bool is_ascii(char c)
{
  // 0xxx xxxx indicates an ascii character.
  return !bit_set_at(8, c);
}

bool is_cont(char c)
{
  // 10xx xxxx indicates this is a continuation byte.
  return bit_set_at(8, c) && !bit_set_at(7, c);
}

// Returns 0 for ASCII characters and -1 for continuations.
int num_bytes_follow(char c)
{
  if (is_ascii(c) || is_cont(c)) {
    return 0;
  }

  // TODO
  // You want to make this more robust by actually checking the other
  // 1's are set, as well as simply checking the not-set bit.
  if (
      bit_set_at(8, c) &&
      bit_set_at(7, c) &&
      !bit_set_at(6, c)
    ) {
    // 110x xxxx One more byte follows
    return 1;
  } else if (
      bit_set_at(8, c) &&
      bit_set_at(7, c) &&
      bit_set_at(6, c) &&
      !bit_set_at(5, c)
    ) {
    // 1110 xxxx Two more bytes follow
    return 2;
  } else if (
      bit_set_at(8, c) &&
      bit_set_at(7, c) &&
      bit_set_at(6, c) &&
      bit_set_at(5, c) &&
      !bit_set_at(4, c)
    ) {
    // 1111 0xxx Three more bytes follow
    return 3;
  } else {
    fprintf(stderr, "WARNING: No case found for num_bytes_follow!\n");
    return -1;
  }
}

bool is_printable(char c)
{
  return (is_ascii(c) && isprint(c)) || !is_cont(c);
}

// UTF8 'parsing' machinery.

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
