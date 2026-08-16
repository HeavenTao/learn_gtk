/* main2.c —— TDouble 类型的测试程序
 * 创建两个 TDouble 对象，测试加、减、乘、除、取负运算，
 * 并演示 "div-by-zero" 信号的连接与触发。
 */

/* 引入 TDouble 头文件、GLib 对象系统与基础库 */
#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

/* 打印一次运算结果：
 * 依次取出三个对象的 double 值（任一失败则直接返回），
 * 按 "v1 op v2 = v3" 的格式输出 */
static void t_print(char *op, TDouble *d1, TDouble *d2, TDouble *d3) {
  double v1, v2, v3;
  if (!t_double_get_value(d1, &v1)) {
    return;
  }
  if (!t_double_get_value(d2, &v2)) {
    return;
  }
  if (!t_double_get_value(d3, &v3)) {
    return;
  }

  g_print("%lf %s %lf=%lf", v1, op, v2, v3);
}

/* "div-by-zero" 信号的回调：
 * 除数为 0 触发信号时调用，向 stderr 打印提示 */
static void div_by_zero_cb(TDouble *self, gpointer user_data) {
  g_printerr("\nHandler in main.c.\n\n");
}

/* 程序入口：测试 TDouble 的四则运算与信号机制 */
int main() {
  TDouble *d1, *d2, *d3;
  double v1, v3;

  /* 创建初值为 10 和 20 的两个 TDouble 对象 */
  d1 = t_double_new(10);
  d2 = t_double_new(20);

  /* 加法 */
  if ((d3 = t_double_add(d1, d2)) != NULL) {
    t_print("+", d1, d2, d3);
    g_object_unref(d3);
  }

  /* 减法 */
  if ((d3 = t_double_sub(d1, d2)) != NULL) {
    t_print("-", d1, d2, d3);
    g_object_unref(d3);
  }

  /* 乘法 */
  if ((d3 = t_double_mul(d1, d2)) != NULL) {
    t_print("*", d1, d2, d3);
    g_object_unref(d3);
  }

  /* 除法 */
  if ((d3 = t_double_div(d1, d2)) != NULL) {
    t_print("/", d1, d2, d3);
    g_object_unref(d3);
  }

  /* 连接 "div-by-zero" 信号到回调，然后把 d2 的值设为 0 以触发除零 */
  g_signal_connect_after(d1, "div-by-zero", G_CALLBACK(div_by_zero_cb), NULL);
  t_double_set_value(d2, 0);
  if ((d3 = t_double_div(d1, d2)) != NULL) {
    t_print("/", d1, d2, d3);
    g_object_unref(d3);
  }

  /* 取负运算，并打印 -v1 = v3 */
  if ((d3 = t_double_uminus(d1)) != NULL && (t_double_get_value(d1, &v1)) &&
      (t_double_get_value(d3, &v3))) {
    g_print("-%lf=%lf\n", v1, v3);
    g_object_unref(d3);
  }

  /* 释放对象引用 */
  g_object_unref(d1);
  g_object_unref(d2);

  return 0;
}
