#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void app_activate(GApplication *app) {
  GtkWidget *win, *nb, *scr, *tv, *label;
  GtkWidget *boxv, *boxh;
  GtkWidget *dmy1, *dmy2, *dmy3;
  GtkWidget *btnn;
  GtkWidget *btno;
  GtkWidget *btns;
  GtkWidget *btnc;
  int i;
  GtkNotebookPage *nbp;
  char *files[] = {"file-1", "file-2"};

  win = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "file editor");
  gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);

  boxv = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(win), boxv);

  boxh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(boxv), boxh);

  dmy1 = gtk_label_new(NULL);
  gtk_label_set_width_chars(GTK_LABEL(dmy1), 10);
  dmy2 = gtk_label_new(NULL);
  gtk_widget_set_hexpand(dmy2, true);
  dmy3 = gtk_label_new(NULL);
  gtk_label_set_width_chars(GTK_LABEL(dmy3), 10);

  btnn = gtk_button_new_with_label("New");
  btno = gtk_button_new_with_label("Open");
  btns = gtk_button_new_with_label("Save");
  btnc = gtk_button_new_with_label("Close");

  gtk_box_append(GTK_BOX(boxh), dmy1);
  gtk_box_append(GTK_BOX(boxh), btnn);
  gtk_box_append(GTK_BOX(boxh), btno);
  gtk_box_append(GTK_BOX(boxh), dmy2);
  gtk_box_append(GTK_BOX(boxh), btns);
  gtk_box_append(GTK_BOX(boxh), btnc);
  gtk_box_append(GTK_BOX(boxh), dmy3);

  nb = gtk_notebook_new();
  gtk_widget_set_hexpand(nb, TRUE);
  gtk_widget_set_vexpand(nb, TRUE);
  gtk_box_append(GTK_BOX(boxv), nb);

  for (i = 0; i < 2; i++) {
    scr = gtk_scrolled_window_new();
    tv = gtk_text_view_new();
    label = gtk_label_new(files[i]);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv);
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), scr, label);
    nbp = gtk_notebook_get_page(GTK_NOTEBOOK(nb), scr);
    g_object_set(nbp, "tab-expand", TRUE, NULL);
  }
  gtk_window_present(GTK_WINDOW(win));
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int stat;

  app = gtk_application_new("com.github.heaventao.myapp",
                            G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  stat = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return stat;
}
