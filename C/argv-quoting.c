#include <stdio.h>

int main(int argc, char** argv) {
  /*
   * Test whether or not quotes (single or double) are kept when passed in as
   * arguments via argv or similar.
   */
  for (int i = 0; i < argc; ++i) {
    printf("%s", argv[i]);
    if (i+1 != argc) {
      printf(", ");
    }
  }
  printf("\n");
}
