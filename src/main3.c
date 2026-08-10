#include "string.h"
#include <stdio.h>
#include <stdlib.h>

static int myLen(char *str) {
  int i = 0;
  char *p = str;
  while (*p != '\0') {
    i++;
    p = p + 1;
  }

  return i;
}

static void myAdd(char *s1, char *s2, char *s3) {
  int len = myLen(s1);
  int len2 = myLen(s2);

  memcpy(s3, s1, len);
  memcpy(s3 + len, s2, len2);

  s3[len + len2] = '\0';
}

int main() {
  char *s = "hello";
  char *s2 = "world";

  int len = myLen(s);
  int len2 = myLen(s2);

  char *s3 = malloc(sizeof(char) * (len + len2 + 1));

  myAdd(s, s2, s3);

  printf("str3 is %s", s3);

  free(s3);
}
