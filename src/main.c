#include "tfetextview.h"

#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

static void tfe_error_alert(GtkWindow *win, GError *err)
{
    GtkAlertDialog *alert_dialog;
    GtkWindow *parent;

    if (win != NULL && gtk_widget_get_mapped(GTK_WIDGET(win)))
    {
        parent = win;
    }
    else
    {
        parent = NULL;
    }

    alert_dialog = gtk_alert_dialog_new("%s", err->message);
    gtk_alert_dialog_show(alert_dialog, parent);
    g_object_unref(alert_dialog);
}

static GtkLabel *tfe_label_from_file(GFile *file)
{
    char *name = "noname";
    if (file == NULL)
    {
        GtkWidget *label = gtk_label_new(name);
        return label;
    }

    char *baseName = g_file_get_basename(file);
    name = g_filename_display_name(baseName);

    GtkWidget *label = gtk_label_new(name);

    g_free(baseName);
    g_free(name);
    return label;
}

static void notebook_page_new_with_file(GtkNotebook *nb, GFile *file)
{
    g_return_if_fail(GTK_IS_NOTEBOOK(nb));
    g_return_if_fail(G_IS_FILE(file) || file == NULL);

    GtkWidget *win;
    GtkNotebookPage *nbp;
    GtkWidget *scr;
    GtkWidget *tv;
    GtkWidget *label;

    int i;
    GError *err = NULL;

    if ((tv = tfe_text_view_new_with_file(file, &err)) == NULL)
    {
        win = gtk_widget_get_ancestor(GTK_WIDGET(nb), GTK_TYPE_WINDOW);
        tfe_error_alert(GTK_WINDOW(win), err);
        g_clear_error(&err);
        return;
    }

    label = tfe_label_from_file(file);
    scr = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), GTK_WIDGET(tv));
    i = gtk_notebook_append_page(nb, scr, label);
    nbp = gtk_notebook_get_page(nb, scr);
    g_object_set(nbp, "tab-expand", TRUE, NULL);
    gtk_notebook_set_current_page(nb, i);
}

static void open_dialog_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GError *err = NULL;
    GFile *file;
    GtkNotebook *nb = GTK_NOTEBOOK(user_data);
    GtkWindow *win = gtk_widget_get_ancestor(GTK_WIDGET(nb), GTK_TYPE_WINDOW);

    file = gtk_file_dialog_open_finish(dialog, res, &err);

    if (file == NULL)
    {
        if (!g_error_matches(err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
        {
            tfe_error_alert(GTK_WINDOW(win), err);
        }
        g_clear_error(&err);
        return;
    }

    notebook_page_new_with_file(nb, file);
    g_object_unref(file);
}

static void save_dialog_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GtkWidget *tv = GTK_WIDGET(user_data);
    GtkWidget *win;
    GtkWidget *scr;
    GtkWidget *nb;
    GtkWidget *label;
    GFile *file;
    GError *err = NULL;

    win = gtk_widget_get_ancestor(tv, GTK_TYPE_WINDOW);
    if ((file = gtk_file_dialog_save_finish(dialog, res, &err)) == NULL)
    {
        if (!g_error_matches(err, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED))
        {
            tfe_error_alert(win, err);
        }
        g_clear_error(&err);
        return;
    }

    tfe_text_view_set_file(TFE_TEXT_VIEW(tv), file);
    label = tfe_label_from_file(file);
    nb = gtk_widget_get_ancestor(tv, GTK_TYPE_NOTEBOOK);
    scr = gtk_widget_get_parent(tv);
    gtk_notebook_set_tab_label(GTK_NOTEBOOK(nb), scr, label);
    g_object_unref(file);

    if (!tfe_text_view_write(TFE_TEXT_VIEW(tv), &err))
    {
        tfe_error_alert(GTK_WINDOW(win), err);
        g_clear_error(&err);
    }
}

static void save_cb(GtkButton *btn, gpointer user_data)
{
    GtkWidget *win;
    GtkNotebook *nb;
    GtkWidget *scr;
    GtkWidget *tv;

    GError *err = NULL;

    GtkFileDialog *file_dialog;
    GtkAlertDialog *alert_dialog;

    int i;

    nb = GTK_NOTEBOOK(user_data);
    win = gtk_widget_get_ancestor(GTK_WIDGET(nb), GTK_TYPE_WINDOW);

    i = gtk_notebook_get_current_page(GTK_NOTEBOOK(nb));

    if (i == -1)
    {
        alert_dialog = gtk_alert_dialog_new("No Page to Save");
        gtk_alert_dialog_show(alert_dialog, GTK_WINDOW(win));
        g_object_unref(alert_dialog);
        return;
    }

    scr = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), i);
    tv = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scr));

    if (tfe_text_view_get_file(TFE_TEXT_VIEW(tv)))
    {
        if (!tfe_text_view_write(TFE_TEXT_VIEW(tv), &err))
        {
            tfe_error_alert(GTK_WINDOW(win), &err);
            g_clear_error(&err);
        }
    }
    else
    {
        file_dialog = gtk_file_dialog_new();
        gtk_file_dialog_open(file_dialog, GTK_WINDOW(win), NULL, save_dialog_cb, tv);
        g_object_unref(file_dialog);
    }
}

static void open_cb(GtkButton *btn, gpointer user_data)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    GtkWidget *nb = GTK_WIDGET(user_data);
    GtkWidget *win = gtk_widget_get_ancestor(nb, GTK_TYPE_WINDOW);

    gtk_file_dialog_open(dialog, GTK_WINDOW(win), NULL, open_dialog_cb, nb);
}

static void new_cb(GtkButton *btn, gpointer user_data)
{
    GtkNotebook *nb = GTK_NOTEBOOK(user_data);
    notebook_page_new_with_file(nb, NULL);
}

static void close_cb(GtkButton *btn, gpointer user_data)
{
    GtkNotebook *nb = GTK_NOTEBOOK(user_data);
    if (gtk_notebook_get_n_pages(nb) >= 2)
    {
    }
    else
    {
        GtkWidget *win = gtk_widget_get_ancestor(GTK_WIDGET(nb), GTK_TYPE_WINDOW);
        gtk_window_destroy(GTK_WINDOW(win));
    }
}

static GtkWidget *get_main_window(GApplication *application)
{
    GtkApplication *app = GTK_APPLICATION(application);
    GList *windows = gtk_application_get_windows(app);
    GtkWidget *win;
    GtkWidget *nb;
    GtkButton *btno;
    GtkButton *btnn;
    GtkButton *btns;
    GtkButton *btnc;

    GtkBuilder *build;

    if (windows)
    {
        return GTK_WIDGET(windows->data);
    }

    build = gtk_builder_new_from_resource("/com/github/heaventao/myapp/main.ui");
    win = GTK_WIDGET(gtk_builder_get_object(build, "win"));
    gtk_window_set_application(GTK_WINDOW(win), app);
    nb = GTK_NOTEBOOK(gtk_builder_get_object(build, "nb"));
    btno = GTK_BUTTON(gtk_builder_get_object(build, "btno"));
    btnn = GTK_BUTTON(gtk_builder_get_object(build, "btnn"));
    btns = GTK_BUTTON(gtk_builder_get_object(build, "btns"));
    btnc = GTK_BUTTON(gtk_builder_get_object(build, "btnc"));

    g_signal_connect(btno, "clicked", G_CALLBACK(open_cb), nb);
    g_signal_connect(btnn, "clicked", G_CALLBACK(new_cb), nb);
    g_signal_connect(btns, "clicked", G_CALLBACK(save_cb), nb);
    g_signal_connect(btnc, "clicked", G_CALLBACK(close_cb), nb);

    g_object_unref(build);
    return win;
}

static GtkWidget *tfe_get_notebook(GtkWidget *win)
{
    GtkWidget *boxv = gtk_window_get_child(GTK_WINDOW(win));
    return gtk_widget_get_last_child(boxv);
}

static void app_activate(GApplication *app)
{
    GtkWindow *win = get_main_window(app);
    GtkWidget *nb = tfe_get_notebook(win);

    notebook_page_new_with_file(GTK_NOTEBOOK(nb), NULL);
    gtk_window_present(win);
}

static void app_open(
    GApplication *self, GFile **files, gint n_files, gchar *hint, gpointer user_data)
{
    GtkWindow *win = get_main_window(self);
    GtkWidget *nb = tfe_get_notebook(win);

    int i;

    for (i = 0; i < n_files; i++)
    {
        notebook_page_new_with_file(GTK_NOTEBOOK(nb), files[i]);
    }

    if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb)) == 0)
    {
        notebook_page_new_with_file(GTK_NOTEBOOK(nb), NULL);
    }

    gtk_window_present(win);
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int stat;
    app = gtk_application_new("com.github.heaventao.myapp", G_APPLICATION_HANDLES_OPEN);

    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    g_signal_connect(app, "open", G_CALLBACK(app_open), NULL);
    stat = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return stat;
}
