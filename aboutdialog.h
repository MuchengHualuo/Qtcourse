#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

#include "studentinfo.h"

class QLabel;
class QPlainTextEdit;

// 自定义的“关于”对话框：显示姓名、学号等个人信息。
// 完全由代码创建界面（不依赖 .ui 文件），与课本 2.4 节的代码化 UI 思路一致。
class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);

    // 直接使用指定的信息（不读 test/about.json）
    explicit AboutDialog(const StudentInfo &info, QWidget *parent = nullptr);

private:
    void buildUi(const StudentInfo &info);
};

#endif // ABOUTDIALOG_H
