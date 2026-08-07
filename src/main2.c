#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

static void t_print(char *op, TDouble *d1, TDouble *d2, TDouble *d3) {
  double v1, v2, v3;

  if (!t_double_get_value(d1, &v1)) {
    return;
  }
  if (!t_double_get_value(d2, &v2)) {
    return;
  }
  if (!t_double_get_value(d3, &v3)) {
    return;
  }

  g_print("%lf %s %lf=%lf", v1, op, v2, v3);
}

int main() {
  TDouble *d1, *d2, *d3;
  double v1, v3;

  d1 = t_double_new(10);
  d2 = t_double_new(20);

  if ((d3 = t_double_add(d1, d2)) != NULL) {
    t_print("+", d1, d2, d3);
  }
}
