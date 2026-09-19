#include "logindialog.h"

#include <QIcon>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

// 演示用的账号密码（课本示例固定为这两项）
static const char *const kDemoUser = "qt";
static const char *const kDemoPassword = "123456";

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    buildUi();
}

void LoginDialog::buildUi()
{
    setWindowTitle(QStringLiteral("登录"));
    setWindowIcon(QIcon(QStringLiteral(":/icons/login-box-line.svg")));
    setFixedWidth(360);

    // ---------------- 标题 ----------------
    auto *titleLabel = new QLabel(QStringLiteral("<h2 style='margin:0;'>混合方式 UI 示例</h2>"), this);
    auto *hintLabel = new QLabel(
        QStringLiteral("<span style='color:#666;'>演示账号：qt　密码：123456</span>"), this);

    // ---------------- 输入框 ----------------
    m_userEdit = new QLineEdit(QStringLiteral("qt"), this);
    m_userEdit->setPlaceholderText(QStringLiteral("请输入用户名"));
    m_userEdit->setClearButtonEnabled(true);

    m_passwordEdit = new QLineEdit(QStringLiteral("123456"), this);
    m_passwordEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);          // 密码用圆点显示
    m_passwordEdit->setClearButtonEnabled(true);

    auto *formLayout = new QFormLayout;
    formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    formLayout->setSpacing(10);
    formLayout->addRow(QStringLiteral("用户名："), m_userEdit);
    formLayout->addRow(QStringLiteral("密　码："), m_passwordEdit);

    // ---------------- 按钮 ----------------
    auto *loginButton = new QPushButton(QIcon(QStringLiteral(":/icons/login-box-line.svg")),
                                        QStringLiteral("登录"), this);
    loginButton->setDefault(true);

    auto *quitButton = new QPushButton(QIcon(QStringLiteral(":/icons/logout-box-line.svg")),
                                       QStringLiteral("退出"), this);

    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(quitButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 16);
    mainLayout->setSpacing(12);
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(hintLabel);
    mainLayout->addSpacing(4);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(quitButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

QString LoginDialog::userName() const
{
    return m_userEdit->text().trimmed();
}

void LoginDialog::onLoginClicked()
{
    if (m_userEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("登录"), QStringLiteral("用户名不能为空！"));
        m_userEdit->setFocus();
        return;
    }

    if (m_passwordEdit->text() != QLatin1String(kDemoPassword)) {
        QMessageBox::warning(this, QStringLiteral("登录"),
                             QStringLiteral("密码错误，请重新输入！"));
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
        return;
    }

    accept();      // 登录成功，main() 里会继续显示主窗口
}
