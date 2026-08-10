# 指定 CMake 的最低版本要求（低于此版本会报错）
cmake_minimum_required(VERSION 3.16)

# 定义工程名称（app），同时隐式设置 C/C++ 编译语言
project(app)

# 查找 PkgConfig 工具（用于定位系统库的编译参数）
find_package(PkgConfig REQUIRED)

# 通过 pkg-config 查找 GTK4 库，并生成一个名为 GTK4 的 IMPORTED 目标
# IMPORTED_TARGET 使链接时自动带上头文件路径和链接库
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# 由 src/main.c 编译生成可执行文件 app
add_executable(app src/main3.c)

# 将 GTK4 库链接到 app 目标（PRIVATE 表示仅 app 自身使用，不对外传播）
target_link_libraries(app PRIVATE PkgConfig::GTK4)

target_compile_options(app PRIVATE -O0)
