#include <stdarg.h>
#include <stdio.h>

static int myf(int count, ...) {

  va_list ap;
  va_start(ap, count);
  int total = 0;
  for (int j = 1; j <= count; j++) {
    total += va_arg(ap, int);
  }
  va_end(ap);

  return total;
}

int main() {
  int x = myf(4, 6, 7, 8, 9);
  printf("%d", x);
}
