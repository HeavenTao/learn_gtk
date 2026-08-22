/* main.c —— 一个基于 GTK4 的简易多标签文本编辑器
 * 自定义 TfeTextView 控件（继承自 GtkTextView），用于在标签页中显示文本，
 * 并在窗口关闭前自动把每个标签页的内容保存回对应的文件。
 */

/* 引入 GIO（文件 I/O、GFile 等）、GLib 基础库、GTK 等头文件 */
#include "tfetextview.h"

#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <string.h>

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <gtk/gtkshortcut.h>

/* 设置该文本视图关联的文件：
 * 先释放旧文件的引用（若存在），再对新文件增加引用（若存在），
 * 最后保存到 tv->file。使用引用计数避免文件对象被提前释放 */

/* activate 信号回调：当应用被激活（如点击图标启动）时触发。
 * 本程序通过 "open" 信号打开文件，因此这里只提示需要文件参数 */

/* close-request 信号回调：窗口请求关闭前调用，把每个标签页的内容保存回文件。
 * 返回 FALSE 表示允许窗口继续关闭 */
static gboolean before_close(GtkWindow *win, GtkWidget *nb)
{
    GtkWidget *scr;          // 滚动窗口
    GtkWidget *tv;           // 文本视图
    GFile *file;             // 当前页关联的文件
    GtkTextBuffer *tb;       // 文本缓冲区
    GtkTextIter start_iter;  // 文本起始迭代器
    GtkTextIter end_iter;    // 文本结束迭代器
    char *contents;          // 缓冲区中的全部文本
    unsigned int n;          // 标签页总数
    unsigned int i;          // 循环变量
    GError *err = NULL;      // 错误信息

    n = gtk_notebook_get_n_pages(GTK_NOTEBOOK(nb));  // 获取标签页数量
    g_print("n is %d\n", n);
    for (i = 0; i < n; ++i)
    {
        scr = gtk_notebook_get_nth_page(GTK_NOTEBOOK(nb), i);          // 取第 i 页（滚动窗口）
        tv = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scr));  // 取滚动窗口中的文本视图
        file = tfe_text_view_get_file(TFE_TEXT_VIEW(tv));              // 取该页关联的文件
        tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));              // 取文本缓冲区
        gtk_text_buffer_get_bounds(tb, &start_iter,
                                   &end_iter);  // 获取整个缓冲区的边界
        contents = gtk_text_buffer_get_text(tb, &start_iter, &end_iter,
                                            FALSE);  // 提取全部文本
        /* 若该页有关联文件，则把文本内容写回文件；失败时打印错误信息 */
        if (file
            && !g_file_replace_contents(file,
                                        contents,
                                        strlen(contents),
                                        NULL,
                                        FALSE,
                                        G_FILE_CREATE_NONE,
                                        NULL,
                                        NULL,
                                        &err))
        {
            g_printerr("%s\n", err->message);
            g_clear_error(&err);
        }
        g_free(contents);                                 // 释放文本内存
        tfe_text_view_set_file(TFE_TEXT_VIEW(tv), NULL);  // 清除文件关联
    }
    return FALSE;  // 允许窗口关闭
}

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
    char *baseName = g_file_get_basename(file);
    char *fileName = g_filename_display_name(baseName);

    GtkWidget *label = gtk_label_new(fileName);
    return label;
}

static void notebook_page_new_with_file(GtkNotebook *nb, GFile *file)
{
    g_return_if_fail(GTK_IS_NOTEBOOK(nb));
    g_return_if_fail(G_IS_FILE(file));

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
    GtkWindow *win = GTK_WINDOW(user_data);
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GError *err = NULL;
    GFile *file;
    GtkNotebook *nb;

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

    /* 复用 app_activate 中已经显示在窗口里的 notebook（存在 win 的数据里） */
    nb = GTK_NOTEBOOK(g_object_get_data(G_OBJECT(win), "nb"));

    notebook_page_new_with_file(nb, file);
    g_object_unref(file);
}

static void open_cb(GtkButton *btn, gpointer user_data)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    GtkWindow *win = GTK_WINDOW(user_data);

    gtk_file_dialog_open(dialog, win, NULL, open_dialog_cb, user_data);
}

static void app_activate(GApplication *app)
{
    GtkWidget *win;  // 主窗口
    GtkBuilder *build;
    GtkWidget *btn_open;

    /* 创建应用窗口并设置标题、默认大小 */
    build = gtk_builder_new_from_resource("/com/github/heaventao/myapp/main.ui");
    win = GTK_WIDGET(gtk_builder_get_object(build, "win"));

    gtk_window_set_title(GTK_WINDOW(win), "hello");
    gtk_window_set_default_size(GTK_WINDOW(win), 500, 400);
    gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));

    btn_open = GTK_WIDGET(gtk_builder_get_object(build, "btn_open"));
    g_signal_connect(btn_open, "clicked", G_CALLBACK(open_cb), win);

    /* 把 notebook 保存在窗口数据中，供 open_dialog_cb 复用 */
    g_object_set_data(G_OBJECT(win), "nb", gtk_builder_get_object(build, "nb"));

    g_object_unref(build);

    gtk_window_present(GTK_WINDOW(win));
}

/* 程序入口 */
int main(int argc, char **argv)
{
    GtkApplication *app;  // 应用对象
    int stat;             // 运行状态码
                          //
                          // g_resources_register(main_get_resource());
    /* 创建 GTK 应用，application id 为 com.github.heaventao.myapp，
     * G_APPLICATION_HANDLES_OPEN 表示应用通过打开文件的方式启动 */
    app = gtk_application_new("com.github.heaventao.myapp", G_APPLICATION_DEFAULT_FLAGS);
    /* 连接 activate 和 open 信号到对应的回调函数 */
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
    // g_signal_connect(app, "open", G_CALLBACK(app_open), NULL);
    stat = g_application_run(G_APPLICATION(app), argc, argv);  // 运行应用主循环

    g_object_unref(app);  // 释放应用对象

    return stat;  // 返回运行状态码
}
