#ifndef UTF8_PARSER_H

#include <stdlib.h>

// N.B.
// This is a bunch of machinery to convert a regular, byte-packed string into
// an internal format slightly 'easier' to print. But theoretically this is bs
// and the bloat can be left for another day.

typedef struct utf8_code_point_t {
  long num_bytes;
  char bytes[];
} UTF8CodePoint;

typedef struct utf8_parse {
  char* remaining_str;
  UTF8CodePoint utf8_code_point;
} UTF8Parse;

enum UTF8ParseResultTy { Ok = 1, Err = 2};

// A Rust-like `Result` type (or Haskell's `Either`.)
// We monomorphise this for simplicity and safety.
typedef struct utf8_parse_result_t {
  enum UTF8ParseResultTy tag;
  char* error;
  UTF8Parse result;
} UTF8ParseResult;

// Main driver for `utf8_parser.c`.
int run_utf8_parser(int argc, char** argv);

// Parse the next code point in the given `base` str
// returning a pair consisting of the parsed code point
// and the remaining string.
UTF8ParseResult parse_next_code_point(char* base);

// smart constructors.

// Construct a parse result object with a result.
UTF8ParseResult utf8_parse_result_result(UTF8ParseResult parse_result);

// Construct a parse result object with an error.
UTF8ParseResult utf8_parse_result_err(char* str);

// random.

void print_utf8_str(char* str, size_t len);

unsigned int shift_unicode_continuation(unsigned char n, size_t amt);

void analyse_bytes_of_str(char* str, size_t len);

void maybe_print_locale_present();

void print_int(int c);

void print_byte(char c);

bool is_ascii(char c);

bool is_cont(char c);

int num_bytes_follow(char c);

char bit_at(size_t pos, int n);

bool bit_set_at(size_t pos, int number);

bool is_printable(char c);

#endif
