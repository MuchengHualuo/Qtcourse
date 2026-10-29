#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("QtCalculator"));
    QApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    MainWindow window;
    window.show();

    return app.exec();
}
