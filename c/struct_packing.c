#include <stdio.h>

typedef struct test1_t
{
  char b;
  double d;
  char b2;
  double d2;
  char b3;
} Test1;

typedef struct test2_t
{
  double d;
  double d2;
  char b;
  char b2;
  char b3;
} Test2;

int main(void)
{
  printf("%ld\n", sizeof(Test1));
  printf("%ld\n", sizeof(Test2));
}
