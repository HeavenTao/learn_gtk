#include <stdio.h>
#include <stdlib.h>
typedef struct stu {
  int age;
} Stu;

Stu *stu_new() {
  Stu *stu = malloc(sizeof(Stu));
  stu->age = 22;
  return stu;
}

void get_stu(Stu **stu) {
  Stu *new_stu = stu_new();

  *stu = new_stu;
}

int main(void) {
  Stu *stu;

  get_stu(&stu);

  printf("the age is %d\n", stu->age);

  free(stu);
}
