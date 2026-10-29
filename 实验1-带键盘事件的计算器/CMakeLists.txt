cmake_minimum_required(VERSION 3.21)

project(QtCalculator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 关闭 CMake 的自动 MOC/UIC/RCC，改由下方 calculator_add_generated_files() 显式调用
# moc/uic/rcc 生成代码：生成过程完全由构建工具直接执行，便于在命令行中复现与排查。
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# 为 target 生成 ui_mainwindow.h、moc_mainwindow.cpp、qrc_resources.cpp。
function(calculator_add_generated_files target)
    set(gen_dir "${CMAKE_CURRENT_BINARY_DIR}/${target}_generated")
    file(MAKE_DIRECTORY "${gen_dir}")

    set(ui_header "${gen_dir}/ui_mainwindow.h")
    add_custom_command(
        OUTPUT "${ui_header}"
        COMMAND $<TARGET_FILE:Qt6::uic> "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui" -o "${ui_header}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui"
        COMMENT "uic: mainwindow.ui -> ui_mainwindow.h"
        VERBATIM)

    set(moc_source "${gen_dir}/moc_mainwindow.cpp")
    add_custom_command(
        OUTPUT "${moc_source}"
        COMMAND $<TARGET_FILE:Qt6::moc> "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h" -o "${moc_source}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h"
        COMMENT "moc: mainwindow.h -> moc_mainwindow.cpp"
        VERBATIM)

    set(qrc_source "${gen_dir}/qrc_resources.cpp")
    add_custom_command(
        OUTPUT "${qrc_source}"
        COMMAND $<TARGET_FILE:Qt6::rcc> "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc"
                -o "${qrc_source}" --name resources
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc"
        COMMENT "rcc: resources.qrc -> qrc_resources.cpp"
        VERBATIM)

    target_sources(${target} PRIVATE "${ui_header}" "${moc_source}" "${qrc_source}")
    target_include_directories(${target} PRIVATE "${gen_dir}")
endfunction()

qt_add_executable(QtCalculator WIN32
    main.cpp
    mainwindow.cpp
    mainwindow.h
    calculatorengine.cpp
    calculatorengine.h
)

calculator_add_generated_files(QtCalculator)

target_link_libraries(QtCalculator PRIVATE Qt6::Widgets)

# 开发辅助工具：按脚本驱动界面并保存截图，用于生成实验报告插图与演示帧。
qt_add_executable(calculator_screenshots
    tools/screenshot_tool.cpp
    mainwindow.cpp
    mainwindow.h
    calculatorengine.cpp
    calculatorengine.h
)

calculator_add_generated_files(calculator_screenshots)

target_include_directories(calculator_screenshots PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(calculator_screenshots PRIVATE Qt6::Widgets)

# ---------------------------------------------------------------------------
# 单元测试：CalculatorEngine 逻辑 + 主窗口键盘事件
# ---------------------------------------------------------------------------
option(CALCULATOR_BUILD_TESTS "Build calculator unit tests" ON)

if(CALCULATOR_BUILD_TESTS)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

    qt_add_executable(tst_calculator
        tests/tst_calculator.cpp
        mainwindow.cpp
        mainwindow.h
        calculatorengine.cpp
        calculatorengine.h
    )

    calculator_add_generated_files(tst_calculator)

    # tst_calculator.cpp 内含 Q_OBJECT 测试类，单独生成 .moc 供其包含
    set(test_gen_dir "${CMAKE_CURRENT_BINARY_DIR}/tst_calculator_generated")
    set(test_moc "${test_gen_dir}/tst_calculator.moc")
    add_custom_command(
        OUTPUT "${test_moc}"
        COMMAND $<TARGET_FILE:Qt6::moc> "${CMAKE_CURRENT_SOURCE_DIR}/tests/tst_calculator.cpp"
                -o "${test_moc}"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/tst_calculator.cpp"
        COMMENT "moc: tests/tst_calculator.cpp"
        VERBATIM)
    set_source_files_properties(tests/tst_calculator.cpp PROPERTIES OBJECT_DEPENDS "${test_moc}")
    target_sources(tst_calculator PRIVATE "${test_moc}")
    target_include_directories(tst_calculator PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        "${test_gen_dir}")

    target_link_libraries(tst_calculator PRIVATE Qt6::Widgets Qt6::Test)

    enable_testing()
    add_test(NAME calculator_unit_tests COMMAND tst_calculator)
endif()

install(TARGETS QtCalculator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)
