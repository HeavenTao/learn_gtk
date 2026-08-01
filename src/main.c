#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void btn_click(GtkButton *btn, GtkWindow *win) { g_print("hello\n"); }

static void app_activate(GApplication *app, gpointer *user_data) {
  GtkWidget *win;
  GtkWidget *label;
  GtkWidget *btn;

  win = gtk_window_new();
  label = gtk_label_new("hello");
  btn = gtk_button_new();

  g_signal_connect(btn, "clicked", G_CALLBACK(btn_click), win);

  gtk_button_set_child(GTK_BUTTON(btn), label);

  gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "HelloWorld");
  gtk_window_set_default_size(GTK_WINDOW(win), 500, 400);
  gtk_window_set_child(GTK_WINDOW(win), btn);
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
