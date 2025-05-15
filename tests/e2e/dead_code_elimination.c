extern int INPUT;
extern int USEVAR;

int i;

void fn() {
  i = INPUT;
}

int main() {
  fn();

  return 0;
}
