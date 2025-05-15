extern int INPUT;
extern int USEVAR;

int i;

void fn() {
  i = 17;
}

int main() {
  fn();
  USEVAR = i;

  return 0;
}
