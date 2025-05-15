#include <stdio.h>

int i, j;

void fn1() {
  int INPUT, USEVAR;
  i = i + 20;
  USEVAR = i;
  printf("%d\n", USEVAR);
}

void fn2() {
  int INPUT, USEVAR;
  i = i + 20;
  USEVAR = i;
  printf("%d\n", USEVAR);
}

int main() {
  int INPUT, USEVAR;
  i = 0;
  j = 0;
  if (INPUT == 0) {
    i = 0;
    fn1();
    fn2();
  } else {
    i = 10;
    fn1();
    fn2();
  }

  if (INPUT == 0) {
    j = 30;
    fn1();
    fn2();
  } else {
    j = 1;
    fn1();
    fn2();
  }

  return 0;
}
