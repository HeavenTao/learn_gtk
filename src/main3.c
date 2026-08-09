#include <ctype.h>
#include <stdio.h>

int main() {
  char s = 'h';

  if (isupper(s)) {
    printf("Yes issupper");
  } else {
    printf("No not supper");
  }
}
