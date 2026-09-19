#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    // 设置当前登录的用户名（显示在状态栏）
    void setCurrentUser(const QString &userName);

    // 触发工具栏上的“关于”动作（等价于鼠标点击该工具按钮），
    // 供自动化测试 / 命令行参数使用。
    void triggerAboutAction();

private slots:
    void on_actionAbout_triggered();      // 工具按钮“关于” <-- 本次新增的功能
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionClear_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
