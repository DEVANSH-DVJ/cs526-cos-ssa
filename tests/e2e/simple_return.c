extern int INPUT;
extern int USEVAR;

int i;

int fn() {
  i = i + 20;
  return i;
}

int main() {
  i = 0;
  USEVAR = fn();

  return 0;
}
