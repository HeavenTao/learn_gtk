/* main.c —— 一个基于 GTK4 的简易多标签文本编辑器
 * 自定义 TfeTextView 控件（继承自 GtkTextView），用于在标签页中显示文本，
 * 并在窗口关闭前自动把每个标签页的内容保存回对应的文件。
 */

/* 引入 GIO（文件 I/O、GFile 等）、GLib 基础库、GTK 等头文件 */
#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>
#include <string.h>

/* 定义 TfeTextView 类型的 get_type 函数宏 */
#define TFE_TYPE_TEXT_VIEW tfe_text_view_get_type()
/* 声明一个 final 类型 TfeTextView，父类型为 GtkTextView，
 * G_DECLARE_FINAL_TYPE 会自动生成实例结构体 typedef 等声明 */
G_DECLARE_FINAL_TYPE(TfeTextView, tfe_text_view, TFE, TEXT_VIEW, GtkTextView);

/* TfeTextView 的私有数据结构：
 * parent —— 继承自 GtkTextView 的父类实例
 * file   —— 与该文本视图关联的文件（GFile），用于保存时写回 */
struct _TfeTextView {
  GtkTextView parent;
  GFile *file;
};

/* 注册 TfeTextView 类型，父类型为 GTK_TYPE_TEXT_VIEW */
G_DEFINE_FINAL_TYPE(TfeTextView, tfe_text_view, GTK_TYPE_TEXT_VIEW);

/* 实例初始化函数：创建实例时把 file 置为 NULL，表示尚未关联文件 */
static void tfe_text_view_init(TfeTextView *tv) { tv->file = NULL; }

/* 类初始化函数：目前不需要初始化类的额外内容，所以为空 */
static void tfe_text_view_class_init(TfeTextViewClass *class) {}

/* 设置该文本视图关联的文件：
 * 先释放旧文件的引用（若存在），再对新文件增加引用（若存在），
 * 最后保存到 tv->file。使用引用计数避免文件对象被提前释放 */
void tfe_text_view_set_file(TfeTextView *tv, GFile *file) {
  if (tv->file) {
    g_object_unref(tv->file);  // 释放旧文件引用
  }
  if (file) {
    g_object_ref(file);  // 增加新文件引用
  }
  tv->file = file;
}

/* 获取该文本视图关联的文件 */
GFile *tfe_text_view_get_file(TfeTextView *tv) { return tv->file; }

/* 创建并返回一个新的 TfeTextView 控件（包装 g_object_new） */
GtkWidget *tfe_text_view_new(void) {
  return GTK_WIDGET(g_object_new(TFE_TYPE_TEXT_VIEW, NULL));
}

/* activate 信号回调：当应用被激活（如点击图标启动）时触发。
 * 本程序通过 "open" 信号打开文件，因此这里只提示需要文件参数 */
static void app_activate(GApplication *app) {
  g_printerr("You need a file argument\n");
}

/* close-request 信号回调：窗口请求关闭前调用，把每个标签页的内容保存回文件。
 * 返回 FALSE 表示允许窗口继续关闭 */
static gboolean before_close(GtkWindow *win, GtkWidget *nb) {
  GtkWidget *scr;       // 滚动窗口
  GtkWidget *tv;        // 文本视图
  GFile *file;          // 当前页关联的文件
  GtkTextBuffer *tb;    // 文本缓冲区
  GtkTextIter start_iter;  // 文本起始迭代器
  GtkTextIter end_iter;    // 文本结束迭代器
  char *contents;       // 缓冲区中的全部文本
  unsigned int n;       // 标签页总数
  unsigned int i;       // 循环变量
  GError *err = NULL;   // 错误信息

  n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb));  // 获取标签页数量
  g_print("n is %d\n", n);
  for (i = 0; i < n; ++i) {
    scr = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), i);  // 取第 i 页（滚动窗口）
    tv = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scr));  // 取滚动窗口中的文本视图
    file = tfe_text_view_get_file(TFE_TEXT_VIEW(tv));  // 取该页关联的文件
    tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));  // 取文本缓冲区
    gtk_text_buffer_get_bounds(tb, &start_iter, &end_iter);  // 获取整个缓冲区的边界
    contents = gtk_text_buffer_get_text(tb, &start_iter, &end_iter, FALSE);  // 提取全部文本
    /* 若该页有关联文件，则把文本内容写回文件；失败时打印错误信息 */
    if (file &&
        !g_file_replace_contents(file, contents, strlen(contents), NULL, FALSE,
                                 G_FILE_CREATE_NONE, NULL, NULL, &err)) {
      g_printerr("%s\n", err->message);
      g_clear_error(&err);
    }
    g_free(contents);  // 释放文本内存
    tfe_text_view_set_file(TFE_TEXT_VIEW(tv), NULL);  // 清除文件关联
  }
  return FALSE;  // 允许窗口关闭
}

/* open 信号回调：当通过命令行参数打开文件时触发。
 * 为每个文件创建一个新的标签页并显示其内容 */
static void app_open(GApplication *app, GFile **files, int n_files,
                     char *hint) {
  GtkWidget *win;  // 主窗口
  GtkWidget *tv;   // 文本视图
  GtkTextBuffer *tb;  // 文本缓冲区
  GtkWidget *scr;  // 滚动窗口
  GtkWidget *nb;   // 标签页容器（Notebook）
  GtkWidget *lab;  // 标签页标题
  GtkNotebookPage *nbp;  // 标签页对象

  char *contents;    // 读取到的文件内容
  gsize length;      // 内容长度
  char *filename_bin, *filename_utf8;  // 文件名（原始字节 / 转为 UTF-8）
  GError *err = NULL;  // 错误信息
  int i = 0;  // 循环变量

  /* 创建应用窗口并设置标题、默认大小 */
  win = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "hello");
  gtk_window_set_default_size(GTK_WINDOW(win), 500, 400);
  nb = gtk_notebook_new();  // 创建 Notebook 作为标签页容器
  gtk_window_set_child(GTK_WINDOW(win), nb);  // 把 Notebook 设为窗口的子控件

  /* 逐个打开命令行传入的文件 */
  for (i = 0; i < n_files; i++) {
    /* 读取文件内容到内存；成功才继续 */
    if (g_file_load_contents(files[i], NULL, &contents, &length, NULL, &err)) {
      scr = gtk_scrolled_window_new();  // 创建滚动窗口
      tv = tfe_text_view_new();  // 创建自定义文本视图
      tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));  // 获取缓冲区
      gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD);  // 按单词换行
      gtk_text_view_set_editable(GTK_TEXT_VIEW(tv), TRUE);  // 允许编辑
      gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv);  // 文本视图放进滚动窗口

      tfe_text_view_set_file(TFE_TEXT_VIEW(tv), files[i]);  // 关联文件
      gtk_text_buffer_set_text(tb, contents, length);  // 把文件内容填入缓冲区
      g_free(contents);  // 释放读取的内容
      filename_bin = g_file_get_basename(files[i]);  // 取文件名（不含路径）
      filename_utf8 = g_filename_display_name(filename_bin);  // 转为适合显示的 UTF-8 形式
      lab = gtk_label_new(filename_utf8);  // 创建标签页标题
      g_free(filename_bin);
      g_free(filename_utf8);
      gtk_notebook_append_page(GTK_NOTEBOOK(nb), scr, lab);  // 把页面加入 Notebook
      nbp = gtk_notebook_get_page(GTK_NOTEBOOK(nb), scr);  // 获取页面对象
      g_object_set(nbp, "tab-expand", TRUE, NULL);  // 设置标签横向铺满，便于多页显示
    } else {
      g_printerr("%s\n", err->message);  // 读取失败，打印错误
      g_clear_error(&err);
    }
  }

  /* 若至少成功打开了一个文件，则连接关闭窗口信号并显示窗口；
   * 否则销毁窗口 */
  if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb)) > 0) {
    g_signal_connect(win, "close-request", G_CALLBACK(before_close), nb);
    gtk_window_present(GTK_WINDOW(win));
  } else {
    gtk_window_destroy(GTK_WINDOW(win));
  }
}

/* 程序入口 */
int main(int argc, char **argv) {
  GtkApplication *app;  // 应用对象
  int stat;  // 运行状态码
  /* 创建 GTK 应用，application id 为 com.github.heaventao.myapp，
   * G_APPLICATION_HANDLES_OPEN 表示应用通过打开文件的方式启动 */
  app = gtk_application_new("com.github.heaventao.myapp",
                            G_APPLICATION_HANDLES_OPEN);
  /* 连接 activate 和 open 信号到对应的回调函数 */
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  g_signal_connect(app, "open", G_CALLBACK(app_open), NULL);
  stat = g_application_run(G_APPLICATION(app), argc, argv);  // 运行应用主循环

  g_object_unref(app);  // 释放应用对象

  return stat;  // 返回运行状态码
}
