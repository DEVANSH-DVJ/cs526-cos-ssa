extern int INPUT1;
extern int INPUT2;
extern int USEVAR;

int i, j;

void fn1() {
  i = i + 20;
  USEVAR = i;
}

void fn2() {
  i = i + 20;
  USEVAR = i;
}

int main() {
  i = 0;
  j = 0;
  if (INPUT1 == 0) {
    i = 0;
    fn1();
    fn2();
  } else {
    i = 10;
    fn1();
    fn2();
  }

  if (INPUT2 == 0) {
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
