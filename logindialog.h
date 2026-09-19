#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;

// 登录对话框：界面全部由代码创建（课本 2.4 节“代码化 UI 设计”部分）。
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    // 用户成功登录后，返回登录的用户名
    QString userName() const;

private slots:
    void onLoginClicked();

private:
    void buildUi();

    QLineEdit *m_userEdit = nullptr;
    QLineEdit *m_passwordEdit = nullptr;
};

#endif // LOGINDIALOG_H
