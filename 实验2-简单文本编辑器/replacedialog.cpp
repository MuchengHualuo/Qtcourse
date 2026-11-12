#include "replacedialog.h"
#include "ui_replacedialog.h"

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    connect(ui->findNextButton, &QPushButton::clicked, this, &ReplaceDialog::onFindNextClicked);
    connect(ui->replaceButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceClicked);
    connect(ui->replaceAllButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAllClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &ReplaceDialog::close);
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::setFindText(const QString &text)
{
    ui->findEdit->setText(text);
    ui->findEdit->selectAll();
    ui->findEdit->setFocus();
}

void ReplaceDialog::showResult(const QString &message)
{
    ui->resultLabel->setText(message);
}

bool ReplaceDialog::checkInput()
{
    if (ui->findEdit->text().isEmpty()) {
        ui->resultLabel->setText(tr("请输入查找内容"));
        return false;
    }
    return true;
}

void ReplaceDialog::onFindNextClicked()
{
    if (!checkInput())
        return;
    emit findNextRequested(ui->findEdit->text(), ui->caseSensitiveCheck->isChecked());
}

void ReplaceDialog::onReplaceClicked()
{
    if (!checkInput())
        return;
    emit replaceRequested(ui->findEdit->text(), ui->replaceEdit->text(),
                          ui->caseSensitiveCheck->isChecked());
}

void ReplaceDialog::onReplaceAllClicked()
{
    if (!checkInput())
        return;
    emit replaceAllRequested(ui->findEdit->text(), ui->replaceEdit->text(),
                             ui->caseSensitiveCheck->isChecked());
}
