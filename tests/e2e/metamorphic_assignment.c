extern int INPUT;
extern int USEVAR;

int i, j;

void fn() {
  i = j;
  USEVAR = i;
}

int main() {
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
