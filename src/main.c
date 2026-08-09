#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string.h>

#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type()
G_DECLARE_FINAL_TYPE(TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView);

struct _TfeTextView {
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_FINAL_TYPE(TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

static void tfe_text_view_init(TfeTextView *tv) { tv->file = NULL; }

static void tfe_text_view_class_init(TfeTextViewClass *class) {}

void tfe_text_view_set_file(TfeTextView *tv, GFile *file) {
  if (tv->file) {
    g_object_unref(tv->file);
  }
  if (file) {
    g_object_ref(file);
  }
  tv->file = file;
}

GFile *tfe_text_view_get_file(TfeTextView *tv) { return tv->file; }

GtkWidget *tfe_text_view_new(void) {
  return GTK_WIDGET(g_object_new(TFE_TYPE_TEXT_VIEW, NULL));
}

static void app_activate(GApplication *app) {
  g_printerr("You need a file argument\n");
}

static gboolean before_close(GtkWindow *win, GtkWidget *nb) {
  GtkWidget *scr;
  GtkWidget *tv;
  GFile *file;
  GtkTextBuffer *tb;
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *contents;
  unsigned int n;
  unsigned int i;
  GError *err = NULL;

  n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb));
  g_print("n is %d\n", n);
  for (i = 0; i < n; ++i) {
    scr = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), i);
    tv = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scr));
    file = tfe_text_view_get_file(TFE_TEXT_VIEW(tv));
    tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
    gtk_text_buffer_get_bounds(tb, &start_iter, &end_iter);
    contents = gtk_text_buffer_get_text(tb, &start_iter, &end_iter, FALSE);
    if (file &&
        g_file_replace_contents(file, contents, strlen(contents), NULL, FALSE,
                                G_FILE_CREATE_NONE, NULL, NULL, &err)) {
      g_printerr("%s\n", err->message);
      g_clear_error(&err);
    }
    g_free(contents);
    tfe_text_view_set_file(TFE_TEXT_VIEW(tv), NULL);
  }
  return FALSE;
}

static void app_open(GApplication *app, GFile **files, int n_files,
                     char *hint) {
  GtkWidget *win;
  GtkWidget *tv;
  GtkTextBuffer *tb;
  GtkWidget *scr;
  GtkWidget *nb;
  GtkWidget *lab;
  GtkNotebookPage *nbp;

  char *contents;
  gsize length;
  char *filename_bin, *filename_utf8;
  GError *err = NULL;
  int i = 0;

  win = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "hello");
  gtk_window_set_default_size(GTK_WINDOW(win), 500, 400);
  nb = gtk_notebook_new();
  gtk_window_set_child(GTK_WINDOW(win), nb);

  for (i = 0; i < n_files; i++) {
    if (g_file_load_contents(files[i], NULL, &contents, &length, NULL, &err)) {
      scr = gtk_scrolled_window_new();
      tv = tfe_text_view_new();
      tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD);
      gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), TRUE);
      gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv);

      tfe_text_view_set_file(TFE_TEXT_VIEW(tv), files[i]);
      gtk_text_buffer_set_text(tb, contents, length);
      g_free(contents);
      filename_bin = g_file_get_basename(files[i]);
      filename_utf8 = g_filename_display_name(filename_bin);
      lab = gtk_label_new(filename_utf8);
      g_free(filename_bin);
      g_free(filename_utf8);
      gtk_notebook_append_page(GTK_NOTEBOOK(nb), scr, lab);
      nbp = gtk_notebook_get_page(GTK_NOTEBOOK(nb), scr);
      g_object_set(nbp, "tab-expand", TRUE, NULL);
    } else {
      g_printerr("%s\n", err->message);
      g_clear_error(&err);
    }

    if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb)) > 0) {
      g_signal_connect(win, "close-request", G_CALLBACK(before_close), nb);
      gtk_window_present(GTK_WINDOW(win));
    } else {
      gtk_window_destroy(GTK_WINDOW(win));
    }
  }
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int stat;
  app = gtk_application_new("com.github.heaventao.myapp",
                            G_APPLICATION_HANDLES_OPEN);
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  g_signal_connect(app, "open", G_CALLBACK(app_open), NULL);
  stat = g_application_run(G_APPLICATION(app), argc, argv);
  g_application_run(G_APPLICATION(app), argc, argv);

  g_object_unref(app);

  return stat;
}
