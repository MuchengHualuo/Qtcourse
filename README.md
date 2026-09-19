# 示例 2.4　混合方式 UI 设计 ＋ 新增“关于”工具按钮

对应视频《Qt 5.9 C++开发指南》**第 5 个分 P（2.4 混合方式 UI 设计）**。
本工程按视频讲解的“混合方式”重建：**主窗口用 Qt Designer 可视化设计，对话框用代码实现**，
并按要求**新增了一个工具栏按钮，点击后弹出“关于”窗口，显示姓名、学号等信息**。

---

## 一、本次新增的功能（作业要求）

| 项目 | 说明 |
| --- | --- |
| 按钮位置 | 主窗口工具栏右侧的 **“关于”** 工具按钮（图标 `shield-user-line`） |
| 图标来源 | [RemixIcon](https://github.com/Remix-Design/RemixIcon)（Apache-2.0），已内置到 `icons/` 并打包进 `res.qrc` |
| 触发方式 | 鼠标点击工具栏按钮，或菜单 **帮助 → 关于（F1）** |
| 弹出内容 | 自定义对话框 `AboutDialog`，显示姓名、学号、班级、专业、学院、开发环境等 |

显示的个人信息：

```
姓　　名：陈良宇
学　　号：2024414290302
班　　级：软件工程三班
专　　业：软件工程
学　　院：计算机学院
```

### 怎么改成你自己的信息（两种方式，任选一种）

1. **不改代码**：直接编辑 `test/about.json`，保存后**无需重新编译**，
   重新运行程序即可生效（构建时会自动把它复制到可执行文件目录）。
2. **改代码**：修改 `studentinfo.cpp` 中 `StudentInfo::defaults()` 里的字符串，重新编译。

> 程序启动时先读取可执行文件目录下的 `test/about.json`，读不到就使用代码里的默认值。

---

## 二、工程结构

```
samp2_4/
├── CMakeLists.txt        构建脚本（Qt 6 官方推荐的 CMake 方式）
├── main.cpp              程序入口：先登录，再显示主窗口
├── mainwindow.ui         主窗口界面（可视化设计，含新增的“关于”动作）
├── mainwindow.h/.cpp     主窗口逻辑（on_actionAbout_triggered 为新增槽函数）
├── logindialog.h/.cpp    登录对话框（全部用代码创建界面）
├── aboutdialog.h/.cpp    “关于”对话框（本次新增，代码创建界面）
├── studentinfo.h/.cpp    个人信息数据（姓名、学号……）
├── res.qrc               资源文件（图标）
├── icons/                RemixIcon 图标（SVG）
│   ├── shield-user-line.svg   ← “关于”按钮
│   ├── folder-open-line.svg   ← 打开
│   ├── draft-line.svg         ← 保存
│   ├── file-copy-line.svg     ← 复制
│   ├── clipboard-line.svg     ← 粘贴
│   ├── delete-bin-line.svg    ← 清空
│   ├── logout-box-line.svg    ← 退出
│   ├── login-box-line.svg     ← 登录
│   └── information-line.svg   ← 提示
└── test/about.json       个人信息配置文件（可编辑，无需重编译）
```

“混合方式”体现在：`MainWindow` 的界面来自 `mainwindow.ui`（Designer 生成），
而 `LoginDialog`、`AboutDialog` 的界面完全由 C++ 代码搭建。

---

## 三、编译与运行

### 环境

- Qt 6.11.2（MinGW 13.1.0 64bit）：`D:\Qt\6.11.2\mingw_64`
- CMake ≥ 3.16、MinGW 工具链：`D:\Qt\Tools\mingw1310_64`、`D:\Qt\Tools\CMake_64`

### 方式一：Qt Creator（推荐）

1. Qt Creator → **打开项目** → 选择 `samp2_4/CMakeLists.txt`；
2. 套件选择 **Desktop Qt 6.11.2 MinGW 64-bit**；
3. 点击 ▶ 运行。启动后用演示账号登录，再点工具栏“关于”按钮。

### 方式二：命令行

```powershell
$env:Path = "D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.11.2\mingw_64\bin;D:\Qt\Tools\CMake_64\bin;$env:Path"

cmake -S . -B build -G "MinGW Makefiles" `
      -DCMAKE_BUILD_TYPE=Release `
      -DCMAKE_MAKE_PROGRAM="D:\Qt\Tools\mingw1310_64\bin\mingw32-make.exe" `
      -DCMAKE_PREFIX_PATH="D:\Qt\6.11.2\mingw_64"

cmake --build build
.\build\samp2_4.exe
```

> 注意：本机 `Ninja` 生成器在此环境下会在 `.ninja_lock` 处卡住，请使用 **MinGW Makefiles**。

如果没有把 Qt 的 `bin` 目录加入 `PATH`，运行 exe 时会提示缺少 `Qt6Widgets.dll`，
把 `D:\Qt\6.11.2\mingw_64\bin` 和 `D:\Qt\Tools\mingw1310_64\bin` 加入 `PATH` 即可；
发布时用 `windeployqt build\samp2_4.exe` 收集依赖。

### 演示账号

| 用户名 | 密码 |
| --- | --- |
| `qt` | `123456` |

### 命令行参数（辅助调试用，正常使用不需要）

| 参数 | 作用 |
| --- | --- |
| `--help` | 显示帮助 |
| `--skip-login` | 跳过登录对话框，直接显示主窗口 |
| `--shot [文件]` | 截屏便于检查界面；带文件名时先自动弹出“关于”对话框 |

---

## 四、实现要点（对应课本 2.4 节）

1. **可视化设计部分**：`mainwindow.ui` 中用 QMainWindow + QPlainTextEdit + QMenuBar +
   QToolBar + QStatusBar 搭好界面；工具栏 `toolButtonStyle` 设为 `ToolButtonTextUnderIcon`，
   图标尺寸 26×26。
2. **新增工具按钮**：在 `.ui` 里新建 `actionAbout`（文本“关于”、图标
   `:/icons/shield-user-line.svg`、快捷键 `F1`），把它加入工具栏和“帮助”菜单，
   Qt Designer 自动生成的 `setupUi()` 会创建对应的 `QToolButton`。
3. **代码化部分**：`AboutDialog` 继承 `QDialog`，在构造函数里用
   `QLabel / QPlainTextEdit / QDialogButtonBox / QVBoxLayout` 布置界面。
4. **信号与槽**：遵循“自动连接”命名规则 `on_<对象名>_<信号名>()`，
   槽函数 `MainWindow::on_actionAbout_triggered()` 无需手写 `connect()`，
   由 `QMetaObject::connectSlotsByName()` 自动关联。
5. **数据与界面分离**：个人信息集中在 `StudentInfo`，界面只负责显示。

---

## 五、图标许可

`icons/` 下的 SVG 图标取自 [RemixIcon](https://github.com/Remix-Design/RemixIcon)，
采用 **Apache License 2.0** 许可，可自由用于学习与商业项目。
文件头已保留其原始路径信息，`res.qrc` 中也标注了来源。
