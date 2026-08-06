#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

struct _TDouble {
  GObject parent;
  double value;
};

static void t_double_class_init(TDoubleClass *class) {
  g_signal_new("div-by-zero", G_TYPE_FROM_CLASS(class),
               G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0,
               NULL, NULL, NULL, G_TYPE_NONE, 0);
}

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

TDouble *t_double_add(TDouble *self, TDouble *other) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);
  g_return_val_if_fail(T_IS_DOUBLE(other), NULL);

  double value;

  if (!t_double_get_value(other, &value)) {
    return NULL;
  }

  return t_double_new(self->value + value);
}
