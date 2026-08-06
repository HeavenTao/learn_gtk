#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

struct _TDouble {
  GObject parent;
  double value;
};

static void t_double_class_init(TDoubleClass *class) {}

static void t_double_init(TDouble *ins) {}

G_DEFINE_TYPE(TDouble, t_double, G_TYPE_OBJECT);

gboolean t_double_get_value(TDouble *self, double *value) {
  g_return_val_if_fail(T_IS_DOUBLE(self), FALSE);

  *value = self->value;
  return TRUE;
}

void t_double_set_value(TDouble *self, double value) {
  g_return_if_fail(T_IS_DOUBLE(self));

  self->value = value;
}

TDouble *t_double_new(double value) {
  TDouble *d;

  d = g_object_new(T_TYPE_DOUBLE, NULL);

  d->value = value;

  return d;
}
