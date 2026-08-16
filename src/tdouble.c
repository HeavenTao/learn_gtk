/* tdouble.c —— 基于 GObject 的自定义双精度浮点数类型 TDouble
 * 封装一个 double 值，提供加、减、乘、除、取负等运算，
 * 并在除数为 0 时发出 "div-by-zero" 信号。
 */

/* 引入本类型的头文件、GLib 对象系统与基础库 */
#include "tdouble.h"
#include <glib-object.h>
#include <glib.h>

/* TDouble 的私有数据结构：
 * parent —— 继承自 GObject 的父类实例
 * value —— 该对象封装的双精度浮点数值 */
struct _TDouble {
  GObject parent;
  double value;
};

/* "div-by-zero" 信号的默认回调：
 * 除数为 0 触发信号时被调用，向 stderr 打印提示信息 */
static void div_by_zero_default_cb(TDouble *self) {
  g_printerr("\ndiv by zero\n");
}

static guint t_double_signal;  // 保存 "div-by-zero" 信号的 id

/* 类初始化函数：注册 "div-by-zero" 信号并关联默认回调 */
static void t_double_class_init(TDoubleClass *class) {
  // 旧式 g_signal_new 注册方式（已注释保留）：
  // t_double_signal =
  //     g_signal_new("div-by-zero", G_TYPE_FROM_CLASS(class),
  //                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE |
  //                  G_SIGNAL_NO_HOOKS, 0, NULL, NULL, NULL, G_TYPE_NONE, 0);

  /* 使用 g_signal_new_class_handler 注册信号：
   * 信号名为 "div-by-zero"，运行阶段为 G_SIGNAL_RUN_LAST，
   * 禁止递归与钩子，默认回调为 div_by_zero_default_cb */
  t_double_signal = g_signal_new_class_handler(
      "div-by-zero", G_TYPE_FROM_CLASS(class),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
      G_CALLBACK(div_by_zero_default_cb), NULL, NULL, NULL, G_TYPE_NONE, 0);
}

/* 实例初始化函数：目前无需额外初始化，保持为空 */
static void t_double_init(TDouble *ins) {}

/* 注册 TDouble 类型，父类型为 G_TYPE_OBJECT */
G_DEFINE_TYPE(TDouble, t_double, G_TYPE_OBJECT);

/* 获取对象封装的 double 值：
 * 成功时把值写入 *value 并返回 TRUE，失败返回 FALSE */
gboolean t_double_get_value(TDouble *self, double *value) {
  g_return_val_if_fail(T_IS_DOUBLE(self), FALSE);

  *value = self->value;
  return TRUE;
}

/* 设置对象封装的 double 值 */
void t_double_set_value(TDouble *self, double value) {
  g_return_if_fail(T_IS_DOUBLE(self));

  self->value = value;
}

/* 创建并返回一个新的 TDouble 对象，初值为 value */
TDouble *t_double_new(double value) {
  TDouble *d;

  d = g_object_new(T_TYPE_DOUBLE, NULL);

  d->value = value;

  return d;
}

/* 二元运算辅助宏：
 * 先取出 other 的值（失败则返回 NULL），
 * 否则用 self->value 与 other 的值做 op 运算并返回新对象 */
#define t_double_binary_op(op)                                                 \
  if (!t_double_get_value(other, &value))                                      \
    return NULL;                                                               \
  return t_double_new(self->value op value);

/* 加法运算：self + other */
TDouble *t_double_add(TDouble *self, TDouble *other) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);
  g_return_val_if_fail(T_IS_DOUBLE(other), NULL);
  double value;

  t_double_binary_op(+);
}

/* 减法运算：self - other */
TDouble *t_double_sub(TDouble *self, TDouble *other) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);
  g_return_val_if_fail(T_IS_DOUBLE(other), NULL);

  double value;

  t_double_binary_op(-);
}

/* 乘法运算：self * other */
TDouble *t_double_mul(TDouble *self, TDouble *other) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);
  g_return_val_if_fail(T_IS_DOUBLE(other), NULL);

  double value;

  t_double_binary_op(*);
}

/* 除法运算：self / other；
 * 若 other 为 0，则发出 "div-by-zero" 信号并返回 NULL */
TDouble *t_double_div(TDouble *self, TDouble *other) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);
  g_return_val_if_fail(T_IS_DOUBLE(other), NULL);

  double value;

  if ((!t_double_get_value(other, &value))) {
    return NULL;
  } else if (value == 0) {
    g_signal_emit(self, t_double_signal, 0);
    return NULL;
  }

  return t_double_new(self->value / value);
}

/* 取负运算：-self */
TDouble *t_double_uminus(TDouble *self) {
  g_return_val_if_fail(T_IS_DOUBLE(self), NULL);

  return t_double_new(-self->value);
}
