cmake_minimum_required(VERSION 3.16)

project(samp2_4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 6 官方推荐：开启 AUTOUIC / AUTOMOC / AUTORCC，
# .ui 文件、Q_OBJECT、.qrc 资源都会在构建时自动处理。
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(samp2_4
    WIN32                 # 生成 GUI 程序（不带控制台窗口）
    main.cpp
    mainwindow.h
    mainwindow.cpp
    mainwindow.ui
    studentinfo.h
    studentinfo.cpp
    logindialog.h
    logindialog.cpp
    aboutdialog.h
    aboutdialog.cpp
    res.qrc
)

target_link_libraries(samp2_4 PRIVATE Qt6::Widgets)

# 个人信息配置文件：改 test/about.json 即可，无需重新编译。
# 构建后自动复制到可执行文件目录（可执行文件目录/test/about.json）。
add_custom_command(TARGET samp2_4 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:samp2_4>/test"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/test/about.json"
            "$<TARGET_FILE_DIR:samp2_4>/test/about.json"
    COMMENT "复制 test/about.json 到可执行文件目录"
)
