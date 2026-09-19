#include "aboutdialog.h"
#include "logindialog.h"
#include "mainwindow.h"
#include "studentinfo.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QIcon>
#include <QPixmap>
#include <QScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(QStringLiteral("samp2_4"));
    app.setApplicationDisplayName(QStringLiteral("示例 2.4  混合方式 UI 设计"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setOrganizationName(QStringLiteral("Qt 5.9 C++开发指南 示例"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/shield-user-line.svg")));

    // ---- 下面两个开关只是为了自动验证 / 演示，正常运行时不需要加参数 ----
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("示例 2.4：混合方式 UI 设计"));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption skipLoginOption(QStringLiteral("skip-login"),
        QStringLiteral("跳过登录对话框，直接显示主窗口"));
    QCommandLineOption shotOption(QStringLiteral("shot"),
        QStringLiteral("截屏到 <文件>；省略文件名时只截主窗口（用于自动化检查）"),
        QStringLiteral("文件"),
        QStringLiteral("-"));
    parser.addOption(skipLoginOption);
    parser.addOption(shotOption);
    parser.process(app);

    const bool skipLogin = parser.isSet(skipLoginOption) || parser.isSet(shotOption);

    QString userName = QStringLiteral("qt");
    if (!skipLogin) {
        // 启动时先显示模态登录对话框，登录成功后才创建主窗口
        LoginDialog login;
        if (login.exec() != QDialog::Accepted)
            return 0;
        userName = login.userName();
    }

    MainWindow window;
    window.setCurrentUser(userName);
    window.show();

    if (parser.isSet(shotOption)) {
        const QString value = parser.value(shotOption);
        const bool mainWindowOnly = (value == QLatin1String("-"));
        const QString target = mainWindowOnly
            ? QStringLiteral("mainwindow-shot.png")
            : value;

        // 自动触发工具栏上的“关于”按钮，等对话框显示后把整屏截图保存下来
        QTimer::singleShot(900, &window, [&window, target, mainWindowOnly]() {
            QTimer::singleShot(1400, &window, [target]() {
                if (QScreen *screen = QApplication::primaryScreen())
                    screen->grabWindow(0).save(target);
                QApplication::quit();
            });
            if (!mainWindowOnly)
                window.triggerAboutAction();   // 模态显示 About 对话框
        });
    }

    return app.exec();
}
