#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

namespace Ui {
class FindDialog;
}

// 查找对话框：只负责收集查找条件，真正的查找由主窗口完成
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);
    ~FindDialog();

    void setFindText(const QString &text);
    void showResult(const QString &message);

signals:
    void findNextRequested(const QString &text, bool caseSensitive);

private slots:
    void onFindNextClicked();

private:
    Ui::FindDialog *ui;
};

#endif // FINDDIALOG_H
