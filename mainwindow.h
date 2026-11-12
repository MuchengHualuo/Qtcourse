#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QLabel;
class QTimer;
class FindDialog;
class ReplaceDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool loadFile(const QString &fileName);   // 打开指定路径的文件（供命令行参数使用）

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    // 格式设置
    void chooseFont();
    void setTextColor();
    void setBackgroundColor();
    void setWordWrap(bool enabled);
    // 视图
    void setToolBarVisible(bool visible);
    void setStatusBarVisible(bool visible);
    void setLineNumberVisible(bool visible);
    // 窗口与状态
    void showAbout();
    void showFindDialog();
    void showReplaceDialog();
    void updateCursorPosition();
    void updateModificationState();
    void updateActionStates();
    void autoSave();
    // 由查找/替换对话框触发
    void findNext(const QString &text, bool caseSensitive);
    void replaceOne(const QString &text, const QString &replacement, bool caseSensitive);
    void replaceAll(const QString &text, const QString &replacement, bool caseSensitive);

private:
    bool maybeSave();                          // 关闭/新建/打开前询问是否保存
    bool saveToFile(const QString &fileName);  // 真正写文件
    void setCurrentFile(const QString &fileName);
    void updateWindowTitle();
    bool findFrom(const QString &text, bool caseSensitive, bool &wrapped);  // 返回是否找到
    void initStatusBar();
    void connectActions();

    Ui::MainWindow *ui;
    QString m_currentFile;
    FindDialog *m_findDialog = nullptr;
    ReplaceDialog *m_replaceDialog = nullptr;
    QTimer *m_autoSaveTimer = nullptr;
    QLabel *m_positionLabel = nullptr;
    QLabel *m_infoLabel = nullptr;
    QLabel *m_stateLabel = nullptr;
};

#endif // MAINWINDOW_H
