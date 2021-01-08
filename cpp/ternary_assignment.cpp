#include <iostream>

int
main(int argc, char** argv)
{
  int a = 0;
  int b = 0;

  (argc == 2 ? a : b) = 5;

  std::cout << "a: " << a << "\n";
  std::cout << "b: " << b << "\n";

  return 0;
}


