#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

int main() {
  TDouble *d;
  TDouble *d2;
  TDouble *d3;
  double value;

  d = t_double_new(10);
  if (t_double_get_value(d, &value)) {
    g_print("t_double_get_value success,the value is %lf\n", value);
  } else {
    g_print("t_double_get_value fail");
  }

  t_double_set_value(d, 20);
  if (t_double_get_value(d, &value)) {
    g_print("t_double_get_value success,the value is %lf\n", value);
  } else {
    g_print("t_double_get_value fail");
  }

  d2 = t_double_new(-10);

  d3 = t_double_add(d, d2);

  if (t_double_get_value(d3, &value)) {
    g_print("d3 value is %lf", value);
  }

  g_object_unref(d);
  g_object_unref(d2);
  g_object_unref(d3);

  return 0;
}
