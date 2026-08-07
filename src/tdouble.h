#pragma once

#include <glib-object.h>
#define T_TYPE_DOUBLE (t_double_get_type())
G_DECLARE_FINAL_TYPE(TDouble, t_double, T, DOUBLE, GObject);

gboolean t_double_get_value(TDouble *self, double *value);

void t_double_set_value(TDouble *self, double value);

TDouble *t_double_new(double value);

TDouble *t_double_add(TDouble *self, TDouble *other);

TDouble *t_double_sub(TDouble *self, TDouble *other);

TDouble *t_double_mul(TDouble *self, TDouble *other);

TDouble *t_double_div(TDouble *self, TDouble *other);

TDouble *t_double_uminus(TDouble *self);
