# 指定 CMake 的最低版本要求（低于此版本会报错）
cmake_minimum_required(VERSION 3.16)

# 定义工程名称（app），同时隐式设置 C/C++ 编译语言
project(app)

# 导出 compile_commands.json，供 clangd 等工具获取真实编译参数
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找 PkgConfig 工具（用于定位系统库的编译参数）
find_package(PkgConfig REQUIRED)

# 通过 pkg-config 查找 GTK4 库，并生成一个名为 GTK4 的 IMPORTED 目标
# IMPORTED_TARGET 使链接时自动带上头文件路径和链接库
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# 查找 glib-compile-resources 工具（GTK/GLib 自带，用于把 .ui 等资源编译成 C 文件）
find_program(GLIB_COMPILE_RESOURCES glib-compile-resources)
if(NOT GLIB_COMPILE_RESOURCES)
  message(FATAL_ERROR "glib-compile-resources not found")
endif()

set(RESOURCES_C ${CMAKE_SOURCE_DIR}/src/resources.c)
add_custom_command(
  OUTPUT ${RESOURCES_C}
  COMMAND ${GLIB_COMPILE_RESOURCES}
          ${CMAKE_SOURCE_DIR}/src/main.gresource.xml
          --sourcedir ${CMAKE_SOURCE_DIR}/src
          --target ${RESOURCES_C}
          --generate-source
  DEPENDS ${CMAKE_SOURCE_DIR}/src/main.gresource.xml
          ${CMAKE_SOURCE_DIR}/src/main.ui
  VERBATIM
)

# 由 src/main.c 编译生成可执行文件 app（resources.c 为自动生成文件）
add_executable(app src/main.c src/tfetextview.c ${RESOURCES_C})

# 将 GTK4 库链接到 app 目标（PRIVATE 表示仅 app 自身使用，不对外传播）
target_link_libraries(app PRIVATE PkgConfig::GTK4)

# -w 关闭所有编译警告，编译时只显示错误信息
# -O0 关闭优化，便于调试
# -g 生成调试信息
# -fdiagnostics-color=always 让错误信息带颜色，更容易区分
# -Werror 将警告视为错误（如需强制消除警告可取消注释）
target_compile_options(app PRIVATE -w -O0 -g -fdiagnostics-color=always)
# target_compile_options(app PRIVATE -Werror)
