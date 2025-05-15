#include <stdio.h>

int i;

void fn() {
  int INPUT, USEVAR;
  i = 17;
}

int main() {
  int INPUT, USEVAR;
  fn();
  USEVAR = i;
  printf("%d\n", USEVAR);

  return 0;
}
