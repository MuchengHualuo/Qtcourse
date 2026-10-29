# QtCalculator —— 带键盘事件的计算器

Qt 6 Widgets 课程实验 1：使用 Qt Designer 设计界面，实现四则运算、小数、退格、清除，
并支持鼠标与键盘两套输入共用同一套处理逻辑。

## 开发环境

- Windows 10/11 64 位
- Qt 6.11.2 (MinGW 13.1 64-bit)
- CMake 3.30 / Ninja
- Git

## 构建

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=D:/Qt/6.11.2/mingw_64 -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

## 运行

```powershell
.\build\QtCalculator.exe
```
