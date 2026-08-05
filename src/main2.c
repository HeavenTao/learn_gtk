#include <glib-object.h>
#include <glib.h>

#define T_DOUBLE_TYPE (t_double_get_type())

typedef struct _TDoubleClass TDoubleClass;
struct _TDoubleClass {
  GObjectClass parent_class;
};

typedef struct _TDouble TDouble;
struct _TDouble {
  GObject parent;
  double value;
};

static void t_double_class_init(TDoubleClass *class) {}

static void t_double_init(TDouble *ins) {}

G_DEFINE_TYPE(TDouble, t_double, G_TYPE_OBJECT);

int main() {
  GType dtype;
  TDouble *d;

  dtype = t_double_get_type();
  if (dtype) {
    g_print("RegisterClass was success!");
  } else {
    g_print("Reg fail");
  }

  d = g_object_new(T_DOUBLE_TYPE, NULL);
  if (d) {
    g_print("instance init success!");
  } else {
    g_print("instance init fail");
  }

  g_object_unref(d);

  return 0;
}
