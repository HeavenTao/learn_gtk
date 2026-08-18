#include "tfetextview.h"

struct _TfeTextView {
  GtkTextView parent;
  GFile *file;
};

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

/* 创建并返回一个新的 TfeTextView 控件（包装 g_object_new） */
GtkWidget *tfe_text_view_new(void) {
  return GTK_WIDGET(g_object_new(TFE_TYPE_TEXT_VIEW, NULL));
}
