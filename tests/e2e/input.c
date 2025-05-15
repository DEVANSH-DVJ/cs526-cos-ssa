#include <stdio.h>

int i;

void fn() {
  int INPUT, USEVAR;
  i = INPUT;
}

int main() {
  int INPUT, USEVAR;
  fn();
  USEVAR = i;

  return 0;
}
