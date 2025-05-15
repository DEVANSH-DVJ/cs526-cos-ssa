extern int INPUT;
extern int USEVAR;

int i;

int fn() {
  /*i = i + 20;*/
  return i;
}

int main() {
  if (INPUT == 0) {
    i = 0;
    USEVAR = fn();
  } else if (INPUT == 1) {
    i = 10;
    USEVAR = fn();
  } else {
    i = 20;
    USEVAR = fn();
  }
  /*USEVAR = 1 + fn();*/

  return 0;
}
