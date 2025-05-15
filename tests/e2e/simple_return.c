#include <stdio.h>

int i;

int fn() {
  int INPUT, USEVAR;
  i = i + 20;
  return i;
}

int main() {
  int INPUT, USEVAR;
  i = 0;
  USEVAR = fn();
  printf("%d\n", USEVAR);

  return 0;
}
