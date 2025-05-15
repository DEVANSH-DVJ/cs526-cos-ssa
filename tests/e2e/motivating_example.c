#include <stdio.h>

int x, w, t;

void P() {
  t = 50;
  x = w + t;
}

int main() {
  int INPUT, USEVAR;
  if (INPUT) {
    w = 10;
    if (USEVAR) {
      P();
      USEVAR = x;
      printf("x = %d\n", USEVAR); // x = 60
      return 0;
    } else {
      P();
      USEVAR = x;
      printf("x = %d\n", USEVAR); // x = 70
    }
  } else {
    w = 20;
    P();
  }
  USEVAR = x;
  printf("x = %d\n", USEVAR); // x = 60 or 70
  return 0;
}
