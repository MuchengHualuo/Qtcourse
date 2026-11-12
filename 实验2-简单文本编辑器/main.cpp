#include "mainwindow.h"

#include <QApplication>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("简单文本编辑器"));
    QApplication::setApplicationVersion(QStringLiteral("1.0"));

    // 载入 Qt 自带的中文翻译，使标准对话框（颜色、字体、消息框）显示中文
    const QString localTranslations = QCoreApplication::applicationDirPath() + QStringLiteral("/translations");
    QTranslator qtTranslator;
    QTranslator qtBaseTranslator;
    if (!qtTranslator.load(QLocale(QLocale::Chinese, QLocale::China), QStringLiteral("qt"),
                           QStringLiteral("_"), localTranslations)) {
        qtTranslator.load(QLocale(QLocale::Chinese, QLocale::China), QStringLiteral("qt"),
                          QStringLiteral("_"), QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    }
    if (!qtBaseTranslator.load(QLocale(QLocale::Chinese, QLocale::China), QStringLiteral("qtbase"),
                               QStringLiteral("_"), localTranslations)) {
        qtBaseTranslator.load(QLocale(QLocale::Chinese, QLocale::China), QStringLiteral("qtbase"),
                              QStringLiteral("_"), QLibraryInfo::path(QLibraryInfo::TranslationsPath));
    }
    app.installTranslator(&qtTranslator);
    app.installTranslator(&qtBaseTranslator);

    MainWindow window;
    window.show();

    // 命令行带有文件路径时（例如右键“用本程序打开”）直接载入该文件
    const QStringList args = QApplication::arguments();
    if (args.size() > 1) {
        const QString path = args.at(1);
        if (QFileInfo::exists(path))
            window.loadFile(path);
    }

    return app.exec();
}
