extern int INPUT;
extern int USEVAR;

int i, j;

void fn1() {
  i = 20;
}

void fn2() {
  i = 10;
}

int main() {
  if (INPUT) {
    fn1();
  } else {
    fn2();
  }

  j = i;
  USEVAR = j;

  return 0;
}
