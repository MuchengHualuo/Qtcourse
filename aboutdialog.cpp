#include "aboutdialog.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFont>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>

namespace {

// 把信息整理成“标签：值”的多行文本，空项自动跳过
QString buildDetailsText(const StudentInfo &info)
{
    const QVector<QPair<QString, QString>> rows = {
        { QStringLiteral("姓　　名"), info.name },
        { QStringLiteral("学　　号"), info.studentId },
        { QStringLiteral("班　　级"), info.className },
        { QStringLiteral("专　　业"), info.major },
        { QStringLiteral("学　　院"), info.school },
    };

    QStringList lines;
    lines << QStringLiteral("程序名称：%1").arg(info.application);
    for (const auto &row : rows) {
        if (!row.second.trimmed().isEmpty())
            lines << QStringLiteral("%1：%2").arg(row.first, row.second);
    }

    lines << QString()
          << QStringLiteral("开发环境：Qt %1").arg(QLatin1String(qVersion()));
    lines << QStringLiteral("构建方式：CMake + Qt Widgets");
    lines << QStringLiteral("图标来源：RemixIcon (Apache-2.0)");

    return lines.join(QLatin1Char('\n'));
}

} // namespace

AboutDialog::AboutDialog(QWidget *parent)
    : AboutDialog(StudentInfo::load(), parent)
{
}

AboutDialog::AboutDialog(const StudentInfo &info, QWidget *parent)
    : QDialog(parent)
{
    buildUi(info);
}

void AboutDialog::buildUi(const StudentInfo &info)
{
    setWindowTitle(info.title);
    setWindowIcon(QIcon(QStringLiteral(":/icons/shield-user-line.svg")));
    setMinimumWidth(430);

    // ---------------- 顶部：图标 + 程序名 ----------------
    auto *iconLabel = new QLabel(this);
    iconLabel->setPixmap(QPixmap(QStringLiteral(":/icons/shield-user-line.svg"))
                             .scaled(56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setFixedSize(56, 56);
    iconLabel->setAlignment(Qt::AlignCenter);

    auto *titleLabel = new QLabel(
        QStringLiteral("<h2 style='margin:0;'>%1</h2>").arg(info.application.toHtmlEscaped()),
        this);

    auto *subTitleLabel = new QLabel(
        QStringLiteral("<span style='color:#666;'>《Qt 5.9 C++开发指南》示例程序</span>"),
        this);

    auto *titleLayout = new QVBoxLayout;
    titleLayout->setSpacing(2);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subTitleLabel);
    titleLayout->addStretch();

    auto *headerLayout = new QHBoxLayout;
    headerLayout->setSpacing(14);
    headerLayout->addWidget(iconLabel);
    headerLayout->addLayout(titleLayout, 1);

    // ---------------- 中部：个人信息 ----------------
    auto *detailsEdit = new QPlainTextEdit(buildDetailsText(info), this);
    detailsEdit->setReadOnly(true);
    detailsEdit->setFrameShape(QFrame::NoFrame);
    detailsEdit->setMinimumHeight(210);
    detailsEdit->setStyleSheet(QStringLiteral(
        "QPlainTextEdit {"
        "  background: palette(base);"
        "  border: 1px solid palette(mid);"
        "  border-radius: 6px;"
        "  padding: 10px;"
        "}"));
    QFont mono = detailsEdit->font();
    mono.setPointSizeF(mono.pointSizeF() + 1.0);
    detailsEdit->setFont(mono);

    // ---------------- 底部：确定按钮 ----------------
    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    buttonBox->button(QDialogButtonBox::Ok)->setIcon(
        QIcon(QStringLiteral(":/icons/information-line.svg")));
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 14);
    mainLayout->setSpacing(12);
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(detailsEdit, 1);
    mainLayout->addWidget(buttonBox);
}
