cmake_minimum_required(VERSION 3.16)

# 导出编译数据库，供 clangd 使用（解决头文件被按 C++ 误解析的问题）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 指定使用 clang 编译（必须在 project() 之前设置）
set(CMAKE_C_COMPILER clang)

project(learn_gtk C)

# C 标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 默认构建类型
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# 常见警告
add_compile_options(-Wall -Wextra)

# 编译 src/main.c
# add_executable(app src/main.c)
add_executable(app src/main2.c src/tdouble.c)

# 可选：GTK4 支持（本环境 devenv 已安装 gtk4）
# 如需使用 GTK4，取消下面注释
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
target_link_libraries(app PRIVATE PkgConfig::GTK4)
