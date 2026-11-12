#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::FindDialog)
{
    ui->setupUi(this);

    connect(ui->findNextButton, &QPushButton::clicked, this, &FindDialog::onFindNextClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &FindDialog::close);
    // 在输入框中回车等同于点击“查找下一个”
    connect(ui->findEdit, &QLineEdit::returnPressed, this, &FindDialog::onFindNextClicked);
}

FindDialog::~FindDialog()
{
    delete ui;
}

void FindDialog::setFindText(const QString &text)
{
    ui->findEdit->setText(text);
    ui->findEdit->selectAll();
    ui->findEdit->setFocus();
}

void FindDialog::showResult(const QString &message)
{
    ui->resultLabel->setText(message);
}

void FindDialog::onFindNextClicked()
{
    const QString text = ui->findEdit->text();
    if (text.isEmpty()) {
        ui->resultLabel->setText(tr("请输入查找内容"));
        return;
    }
    emit findNextRequested(text, ui->caseSensitiveCheck->isChecked());
}
