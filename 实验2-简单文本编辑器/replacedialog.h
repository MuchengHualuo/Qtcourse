#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

namespace Ui {
class ReplaceDialog;
}

// 替换对话框：提供“查找下一个 / 替换 / 全部替换”三种操作
class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);
    ~ReplaceDialog();

    void setFindText(const QString &text);
    void showResult(const QString &message);

signals:
    void findNextRequested(const QString &text, bool caseSensitive);
    void replaceRequested(const QString &text, const QString &replacement, bool caseSensitive);
    void replaceAllRequested(const QString &text, const QString &replacement, bool caseSensitive);

private slots:
    void onFindNextClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    bool checkInput();          // 检查查找内容是否为空

    Ui::ReplaceDialog *ui;
};

#endif // REPLACEDIALOG_H
