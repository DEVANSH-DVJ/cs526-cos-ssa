#include <stdio.h>

int i;

int fn() {
  int INPUT, USEVAR;
  /*i = i + 20;*/
  return i;
}

int main() {
  int INPUT, USEVAR;
  if (INPUT == 0) {
    i = 0;
    USEVAR = fn();
    printf("%d\n", USEVAR);
  } else if (INPUT == 1) {
    i = 10;
    USEVAR = fn();
    printf("%d\n", USEVAR);
  } else {
    i = 20;
    USEVAR = fn();
    printf("%d\n", USEVAR);
  }
  /*
  USEVAR = 1 + fn();
  printf("%d\n", USEVAR);
  */

  return 0;
}
