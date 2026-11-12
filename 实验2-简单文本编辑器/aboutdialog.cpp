#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    // “确定”按钮直接关闭对话框
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
