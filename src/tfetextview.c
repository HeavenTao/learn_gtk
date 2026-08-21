#include "tfetextview.h"
#include <gtk/gtk.h>

struct _TfeTextView {
  GtkTextView parent;
  GFile *file;
};

G_DEFINE_QUARK(tfe - text - view - error - quark, tfe_text_view_error);
G_DEFINE_FINAL_TYPE(TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

static void tfe_text_view_dispose(GObject *object) {
  TfeTextView *tv = TFE_TEXT_VIEW(object);

  g_clear_object(&tv->file);

  G_OBJECT_CLASS(tfe_text_view_parent_class)->dispose(object);
}

/* 实例初始化函数：创建实例时把 file 置为 NULL，表示尚未关联文件 */
static void tfe_text_view_init(TfeTextView *tv) { tv->file = NULL; }

/* 类初始化函数：目前不需要初始化类的额外内容，所以为空 */
static void tfe_text_view_class_init(TfeTextViewClass *class) {
  GObjectClass *object_class = G_OBJECT_CLASS(class);

  object_class->dispose = tfe_text_view_dispose;
}

void tfe_text_view_set_file(TfeTextView *tv, GFile *file) {
  if (tv->file) {
    g_object_unref(tv->file); // 释放旧文件引用
  }
  if (file) {
    g_object_ref(file); // 增加新文件引用
  }
  tv->file = file;
}

/* 获取该文本视图关联的文件 */
GFile *tfe_text_view_get_file(TfeTextView *tv) { return tv->file; }

gboolean tfe_text_view_read(TfeTextView *tv, GError **err) { return FALSE; }

gboolean tfe_text_view_write(TfeTextView *tv, GError **err) {
  g_return_val_if_fail(TFE_IS_TEXT_VIEW(tv), FALSE);
  g_return_val_if_fail((err == NULL || *err == NULL), FALSE);

  GtkTextBuffer *tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  char *content;
  gboolean stat;

  if (!G_IS_FILE(tv->file)) {
    g_set_error_literal(err, TFE_TEXT_VIEW_ERROR, TFE_TEXT_VIEW_ERROR_NO_FILE,
                        "No file is set in tfeTextView");
    return FALSE;
  }

  gtk_text_buffer_get_bounds(tb, &start_iter, &end_iter);
  content = gtk_text_buffer_get_text(tb, &start_iter, &end_iter, FALSE);
  stat = g_file_replace_contents(tv->file, content, strlen(content), NULL, TRUE,
                                 G_FILE_CREATE_NONE, NULL, NULL, err);
  g_free(content);
  if (stat) {
    gtk_text_buffer_set_modified(tb, FALSE);
  }
  return stat;
}

/* 创建并返回一个新的 TfeTextView 控件（包装 g_object_new） */
GtkWidget *tfe_text_view_new(void) {
  return GTK_WIDGET(g_object_new(TFE_TYPE_TEXT_VIEW, NULL));
}

GtkWidget *tfe_text_view_new_with_file(GFile *file, GError **err) {
  return NULL;
}
