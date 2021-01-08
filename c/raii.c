#include <stdio.h>
#include <stdlib.h>

/*struct map_t {*/

/*};*/
/*typedef struct map_t Map;*/

/*struct thunk_t {*/
  /*void (*f)();*/
  /*Map env;*/
/*};*/
/*typedef struct thunk_t Thunk;*/

struct raii_t {
  int* rsrc;
  void (*drop)(struct raii_t*);
};
typedef struct raii_t Raii;

void
drop_raii(Raii* raii)
{
  free(raii->rsrc);
}

// Could take this as a valist or could make everything a single struct and make that an array
// however, thedn you lose type safety with the void* to the rsrc
// Although technically you could wrap that up again, so that the Raii's rsrc is pointing to another wrapped type
//void
//cleanup(variadic locals as list)
//{
//  for (each local arg) {
//    call drop on it
//  }
//}

void
test(size_t count)
{
  int* xs =
    malloc(sizeof(int) * count);
  for (size_t i = 0; i < count; ++i) {
    xs[i] = i;
    printf("%d\n", xs[i]);
  }

  // C99 designated initializers.
  Raii raii = {
    .rsrc = xs,
    .drop = &drop_raii,
  };

  printf("Dropping rsrc\n");
  raii.drop(&raii);
}

int
main(void)
{
  test(10);

  exit(EXIT_SUCCESS);
}
