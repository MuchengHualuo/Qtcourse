#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calculatorengine.h"

#include <QHash>
#include <QMainWindow>

class QKeyEvent;
class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief 计算器主窗口。
 *
 * 界面由 Qt Designer 生成的 mainwindow.ui 提供；鼠标点击与键盘按键都被转换为
 * 同一个“输入记号”（token），再交给 handleToken() 处理，从而保证两套输入
 * 完全复用同一套逻辑，不会出现行为差异。
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void applyStyleSheet();
    void connectButtons();
    void configureWidgets();
    void handleToken(const QString &token);
    void refreshDisplay();
    void setButtonDown(const QString &token, bool pressed);

    Ui::MainWindow *ui;
    CalculatorEngine m_engine;
    QHash<QString, QPushButton *> m_buttonByToken;
};

#endif // MAINWINDOW_H
