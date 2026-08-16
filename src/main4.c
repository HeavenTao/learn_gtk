/* main4.c —— 基于 GTK4 的简易文件编辑器界面
 * 创建一个带工具栏按钮（New/Open/Save/Close）的窗口，
 * 并用 Notebook 显示两个可编辑的文本标签页。
 */

/* 引入 GIO、GLib 对象系统、GTK 及快捷键相关头文件 */
#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

/* activate 信号回调：应用被激活时创建主窗口及其界面 */
static void app_activate(GApplication *app) {
  GtkWidget *win, *nb, *scr, *tv, *label;  // 窗口、标签页容器、滚动窗口、文本视图、标签
  GtkWidget *boxv, *boxh;  // 垂直、水平布局容器
  GtkWidget *dmy1, *dmy2, *dmy3;  // 用于调整工具栏间距的占位标签
  GtkWidget *btnn;  // New 按钮
  GtkWidget *btno;  // Open 按钮
  GtkWidget *btns;  // Save 按钮
  GtkWidget *btnc;  // Close 按钮
  int i;  // 循环变量
  GtkNotebookPage *nbp;  // 标签页对象
  char *files[] = {"file-1", "file-2"};  // 两个标签页的标题

  /* 创建应用窗口，设置标题和默认大小 */
  win = gtk_application_window_new(GTK_APPLICATION(app));
  gtk_window_set_title(GTK_WINDOW(win), "file editor");
  gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);

  /* 创建垂直布局，作为窗口的直接子控件 */
  boxv = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(GTK_WINDOW(win), boxv);

  /* 创建水平布局作为工具栏，并放入垂直布局的顶部 */
  boxh = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_append(GTK_BOX(boxv), boxh);

  /* 创建三个占位标签，用于调整工具栏中按钮的间距 */
  dmy1 = gtk_label_new(NULL);
  gtk_label_set_width_chars(GTK_LABEL(dmy1), 10);
  dmy2 = gtk_label_new(NULL);
  gtk_widget_set_hexpand(dmy2, true);  // 中间的占位可水平扩展，把两侧按钮分开
  dmy3 = gtk_label_new(NULL);
  gtk_label_set_width_chars(GTK_LABEL(dmy3), 10);

  /* 创建四个工具栏按钮 */
  btnn = gtk_button_new_with_label("New");
  btno = gtk_button_new_with_label("Open");
  btns = gtk_button_new_with_label("Save");
  btnc = gtk_button_new_with_label("Close");

  /* 按 占位-New-Open-占位-Save-Close-占位 的顺序放入工具栏 */
  gtk_box_append(GTK_BOX(boxh), dmy1);
  gtk_box_append(GTK_BOX(boxh), btnn);
  gtk_box_append(GTK_BOX(boxh), btno);
  gtk_box_append(GTK_BOX(boxh), dmy2);
  gtk_box_append(GTK_BOX(boxh), btns);
  gtk_box_append(GTK_BOX(boxh), btnc);
  gtk_box_append(GTK_BOX(boxh), dmy3);

  /* 创建 Notebook 标签页容器，使其水平和垂直方向都可扩展 */
  nb = gtk_notebook_new();
  gtk_widget_set_hexpand(nb, TRUE);
  gtk_widget_set_vexpand(nb, TRUE);
  gtk_box_append(GTK_BOX(boxv), nb);

  /* 循环创建两个标签页，每个页面包含一个带滚动条的文本编辑器 */
  for (i = 0; i < 2; i++) {
    scr = gtk_scrolled_window_new();
    tv = gtk_text_view_new();
    label = gtk_label_new(files[i]);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(tv), GTK_WRAP_WORD_CHAR);  // 按字符换行
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scr), tv);
    gtk_notebook_append_page(GTK_NOTEBOOK(nb), scr, label);
    nbp = gtk_notebook_get_page(GTK_NOTEBOOK(nb), scr);
    g_object_set(nbp, "tab-expand", TRUE, NULL);  // 让标签横向铺满
  }
  gtk_window_present(GTK_WINDOW(win));  // 显示窗口
}

/* 程序入口：创建 GtkApplication 并运行主循环 */
int main(int argc, char **argv) {
  GtkApplication *app;
  int stat;

  /* 创建 GTK 应用并指定 application id */
  app = gtk_application_new("com.github.heaventao.myapp",
                            G_APPLICATION_DEFAULT_FLAGS);
  /* 连接 activate 信号到 app_activate 回调 */
  g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
  /* 运行应用主循环，阻塞直到应用退出 */
  stat = g_application_run(G_APPLICATION(app), argc, argv);
  /* 释放应用对象 */
  g_object_unref(app);
  return stat;
}
