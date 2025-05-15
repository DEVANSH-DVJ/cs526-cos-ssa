#include <stdio.h>

int i, j;

void fn() {
  int INPUT, USEVAR;
  i = j;
  USEVAR = i;
  printf("%d\n", USEVAR);
}

int main() {
  int INPUT, USEVAR;
  if (INPUT == 0) {
    j = 20;
    fn();
  } else if (INPUT == 1) {
    j = 22;
    fn();
  } else {
    j = 24;
    fn();
  }

  return 0;
}
