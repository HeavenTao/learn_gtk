/* main3.c —— 字符串操作示例程序
 * 手动实现求字符串长度（myLen）和字符串拼接（myAdd），
 * 演示 C 语言中字符串与动态内存的基本操作。
 */

/* 引入字符串处理、标准输入输出、动态内存等头文件 */
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

/* 计算字符串 str 的长度（不含结尾的 '\0'） */
static int myLen(char *str) {
  int i = 0;
  char *p = str;

  /* 从字符串头部开始逐个字符向后遍历，直到遇到 '\0' */
  while (*p != '\0') {
    i++;
    p = p + 1;
  }

  return i;
}

/* 把字符串 s1 和 s2 依次拼接后存入 s3：
 * s3 必须预先分配好至少 len(s1)+len(s2)+1 字节的空间 */
static void myAdd(char *s1, char *s2, char *s3) {
  int len = myLen(s1);
  int len2 = myLen(s2);

  /* 先把 s1 复制到 s3 的开头 */
  memcpy(s3, s1, len);
  /* 再把 s2 复制到 s1 之后 */
  memcpy(s3 + len, s2, len2);

  /* 在末尾补上字符串结束符 */
  s3[len + len2] = '\0';
}

/* 程序入口：拼接 "hello" 和 "world" 并输出结果 */
int main() {
  char *s = "hello";
  char *s2 = "world";

  int len = myLen(s);
  int len2 = myLen(s2);

  /* 分配能容纳 s + s2 + 结束符 的堆内存 */
  char *s3 = malloc(sizeof(char) * (len + len2 + 1));

  /* 拼接字符串 */
  myAdd(s, s2, s3);

  /* 输出拼接结果 */
  printf("str3 is %s", s3);

  /* 释放动态分配的内存，避免内存泄漏 */
  free(s3);
}
