#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calculatorengine.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief 计算器主窗口：负责界面呈现，把按钮点击转换为计算器引擎的输入动作。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void connectButtons();
    void handleToken(const QString &token);
    void refreshDisplay();

    Ui::MainWindow *ui;
    CalculatorEngine m_engine;
};

#endif // MAINWINDOW_H
