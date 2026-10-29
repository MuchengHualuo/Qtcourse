/**
 * @file screenshot_tool.cpp
 * @brief 界面截图/演示录制辅助工具（开发调试用，不参与计算器正式功能）。
 *
 * 通过一个简单的文本脚本驱动主窗口，支持“点击某个按钮”“按下/松开某个键盘键”
 * “保存当前窗口截图”三类指令，用于批量生成实验报告所需的运行截图与演示帧。
 *
 * 用法：
 *   calculator_screenshots --script demo.txt --out shots
 *
 * 脚本示例（# 开头为注释）：
 *   click btn1
 *   click btn2
 *   click btnDot
 *   snap 01_input
 *   key Enter
 *   snap 02_result
 */
#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QKeyEvent>
#include <QPushButton>
#include <QTextStream>

namespace {

/// 把脚本中的键名转换成 Qt 键值；返回 0 表示无法识别。
int keyValue(const QString &name)
{
    if (name.size() == 1) {
        const QChar c = name.at(0);
        if (c.isDigit())
            return Qt::Key_0 + c.digitValue();
        switch (c.unicode()) {
        case '.': return Qt::Key_Period;
        case '+': return Qt::Key_Plus;
        case '-': return Qt::Key_Minus;
        case '*': return Qt::Key_Asterisk;
        case '/': return Qt::Key_Slash;
        case '=': return Qt::Key_Equal;
        default: break;
        }
    }

    if (name.compare(QLatin1String("Enter"), Qt::CaseInsensitive) == 0)
        return Qt::Key_Return;
    if (name.compare(QLatin1String("Backspace"), Qt::CaseInsensitive) == 0)
        return Qt::Key_Backspace;
    if (name.compare(QLatin1String("Escape"), Qt::CaseInsensitive) == 0)
        return Qt::Key_Escape;
    if (name.compare(QLatin1String("Tab"), Qt::CaseInsensitive) == 0)
        return Qt::Key_Tab;
    if (name.compare(QLatin1String("Delete"), Qt::CaseInsensitive) == 0)
        return Qt::Key_Delete;
    return 0;
}

QString keyText(int key)
{
    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        return QString(QChar('0' + (key - Qt::Key_0)));
    switch (key) {
    case Qt::Key_Period: return QStringLiteral(".");
    case Qt::Key_Plus: return QStringLiteral("+");
    case Qt::Key_Minus: return QStringLiteral("-");
    case Qt::Key_Asterisk: return QStringLiteral("*");
    case Qt::Key_Slash: return QStringLiteral("/");
    case Qt::Key_Equal: return QStringLiteral("=");
    case Qt::Key_Return: return QStringLiteral("\r");
    default: return QString();
    }
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString scriptPath;
    QString outDir = QStringLiteral("shots");
    const QStringList args = QApplication::arguments();
    for (int i = 1; i < args.size(); ++i) {
        if (args.at(i) == QLatin1String("--script") && i + 1 < args.size())
            scriptPath = args.at(++i);
        else if (args.at(i) == QLatin1String("--out") && i + 1 < args.size())
            outDir = args.at(++i);
    }

    if (scriptPath.isEmpty()) {
        QTextStream(stderr) << "usage: calculator_screenshots --script <file> --out <dir>\n";
        return 2;
    }

    QFile script(scriptPath);
    if (!script.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream(stderr) << "cannot open script: " << scriptPath << "\n";
        return 2;
    }

    QDir().mkpath(outDir);

    MainWindow window;
    window.resize(380, 600);
    window.show();
    QApplication::processEvents();

    QTextStream in(&script);
    int lineNumber = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        ++lineNumber;
        if (line.isEmpty() || line.startsWith(QLatin1Char('#')))
            continue;

        const QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        const QString command = parts.value(0);

        if (command == QLatin1String("click") && parts.size() >= 2) {
            auto *button = window.findChild<QPushButton *>(parts.at(1));
            if (!button) {
                QTextStream(stderr) << "line " << lineNumber << ": no button " << parts.at(1) << "\n";
                return 3;
            }
            button->click();
        } else if (command == QLatin1String("key") && parts.size() >= 2) {
            const int key = keyValue(parts.at(1));
            if (key == 0) {
                QTextStream(stderr) << "line " << lineNumber << ": unknown key " << parts.at(1) << "\n";
                return 3;
            }
            QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier, keyText(key));
            QApplication::sendEvent(&window, &press);
            QKeyEvent release(QEvent::KeyRelease, key, Qt::NoModifier, keyText(key));
            QApplication::sendEvent(&window, &release);
        } else if (command == QLatin1String("keydown") && parts.size() >= 2) {
            const int key = keyValue(parts.at(1));
            QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier, keyText(key));
            QApplication::sendEvent(&window, &press);
        } else if (command == QLatin1String("keyup") && parts.size() >= 2) {
            const int key = keyValue(parts.at(1));
            QKeyEvent release(QEvent::KeyRelease, key, Qt::NoModifier, keyText(key));
            QApplication::sendEvent(&window, &release);
        } else if (command == QLatin1String("snap") && parts.size() >= 2) {
            QApplication::processEvents();
            const QPixmap shot = window.grab();
            const QString filePath = QDir(outDir).filePath(parts.at(1) + QStringLiteral(".png"));
            if (!shot.save(filePath)) {
                QTextStream(stderr) << "failed to save " << filePath << "\n";
                return 4;
            }
            QTextStream(stdout) << "saved " << filePath << " (" << shot.width() << "x"
                                << shot.height() << ")\n";
        } else if (command == QLatin1String("wait") && parts.size() >= 2) {
            QApplication::processEvents();
        } else {
            QTextStream(stderr) << "line " << lineNumber << ": bad command " << line << "\n";
            return 3;
        }
        QApplication::processEvents();
    }

    return 0;
}
