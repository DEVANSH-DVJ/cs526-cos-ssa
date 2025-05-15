#include <stdio.h>

int i, j;

void fn1() {
  int INPUT, USEVAR;
  i = 20;
}

void fn2() {
  int INPUT, USEVAR;
  i = 10;
}

int main() {
  int INPUT, USEVAR;
  if (INPUT) {
    fn1();
  } else {
    fn2();
  }

  j = i;
  USEVAR = j;
  printf("%d\n", USEVAR);

  return 0;
}
