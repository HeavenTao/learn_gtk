/* main4.c —— 基于 GTK4 的简易文件编辑器界面
 * 创建一个带工具栏按钮（New/Open/Save/Close）的窗口，
 * 并用 Notebook 显示两个可编辑的文本标签页。
 */

/* 引入 GIO、GLib 对象系统、GTK 及快捷键相关头文件 */
#include "resources.c"
#include <gio/gio.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

/* activate 信号回调：应用被激活时创建主窗口及其界面 */
static void app_activate(GApplication *app) {
  GtkWidget *win;
  GtkWidget *nb;
  GtkWidget *scr;
  GtkNotebookPage *nbp;
  int n_pages;
  GtkBuilder *build;

  build = gtk_builder_new_from_resource("/com/github/heaventao/myapp/main4.ui");
  win = GTK_WIDGET(gtk_builder_get_object(build, "win"));
  nb = GTK_WIDGET(gtk_builder_get_object(build, "nb"));
  gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));
  g_object_unref(build);

  n_pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb));

  for (int i = 0; i < n_pages; i++) {
    scr = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), i);
    nbp = gtk_notebook_get_page(GTK_NOTEBOOK(nb), scr);
    g_object_set(nbp, "tab-expand", TRUE, NULL);
  }

  gtk_window_present(GTK_WINDOW(win));
}

/* 程序入口：创建 GtkApplication 并运行主循环 */
int main(int argc, char **argv) {
  GtkApplication *app;
  int stat;

  /* 将编译进程序的 GResource 注册到全局资源表（否则按路径查不到 main4.ui） */
  g_resources_register(main4_get_resource());

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
