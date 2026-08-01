#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void hellobtn_clicked(GtkButton *btn, GtkWindow *win) {
  const char *s;
  s = gtk_button_get_label(btn);
  if (g_strcmp0(s, "hello") == 0) {
    gtk_button_set_label(btn, "goodbye");
  } else {
    gtk_button_set_label(btn, "hello");
  }
}

static void closebtn_clicked(GtkButton *btn, GtkWindow *win) {
  gtk_window_destroy(win);
}

static void app_activate(GApplication *app) {
  GtkWidget *win;
  GtkWidget *box;
  GtkWidget *btn1;
  GtkWidget *btn2;

  win = gtk_window_new();
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  btn1 = gtk_button_new_with_label("hello");
  g_signal_connect(btn1, "clicked", G_CALLBACK(hellobtn_clicked), win);
  btn2 = gtk_button_new_with_label("close");
  g_signal_connect(btn2, "clicked", G_CALLBACK(closebtn_clicked), win);

  gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
  gtk_box_append(GTK_BOX(box), btn1);
  gtk_box_append(GTK_BOX(box), btn2);

  gtk_window_set_child(GTK_WINDOW(win), box);

  gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "HelloWorld");
  gtk_window_set_default_size(GTK_WINDOW(win), 500, 400);
  gtk_window_present(GTK_WINDOW(win));

  g_print("GtkApplication is created");
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int stat;
  app = gtk_application_new("com.github.heaventao.myapp",
                            G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  stat = g_application_run(G_APPLICATION(app), argc, argv);
  g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);

  return stat;
}
