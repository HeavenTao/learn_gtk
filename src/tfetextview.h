#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

G_DECLARE_FINAL_TYPE(TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView);

#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type()
#define TFE_TEXT_VIEW_ERROR (tfe_text_view_error_quark())
GQuark tfe_text_view_error_quark(void);

typedef enum {
  TFE_TEXT_VIEW_ERROR_NO_FILE,
  TFE_TEXT_VIEW_ERROR_FAILED
} TfeTextViewError;

void tfe_text_view_set_file(TfeTextView *tv, GFile *file);

GFile *tfe_text_view_get_file(TfeTextView *tv);

GtkWidget *tfe_text_view_new(void);

void tfe_error_alert(GtkWindow *win, GError *err);

gboolean tfe_text_view_write(TfeTextView *tv, GError **err);
