#include <glib-object.h>
#include <glib.h>

typedef struct _TDoubleClass TDoubleClass;
struct _TDoubleClass {
  GObjectClass parent_class;
};

typedef struct _TDouble TDouble;
struct _TDouble {
  GObject parent;
  double value;
};

static void show_ref_count(GObject *instance) {
  if (G_IS_OBJECT(instance)) {
    g_print("Ref count is %d\n", instance->ref_count);
  } else {
    g_print("Instance is not object");
  }
}

int main() {
  GObject *instance;

  instance = g_object_new(G_TYPE_OBJECT, NULL);

  show_ref_count(instance);

  g_object_ref(instance);

  show_ref_count(instance);

  g_object_unref(instance);

  show_ref_count(instance);

  g_object_unref(instance);

  show_ref_count(instance);
}
