extern int INPUT;
extern int USEVAR;

int i;

void fn() {
  i = INPUT;
}

int main() {
  fn();
  USEVAR = i;

  return 0;
}
