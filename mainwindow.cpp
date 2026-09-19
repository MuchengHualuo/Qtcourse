#include "mainwindow.h"
#include "aboutdialog.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFont>
#include <QIcon>
#include <QMessageBox>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 工具栏与菜单里各动作的图标都来自 RemixIcon，
    // 已在 mainwindow.ui 中指定（图标文件在 res.qrc 里注册）。
    QFont font = ui->plainTextEdit->font();
    font.setPointSizeF(font.pointSizeF() + 1.5);
    ui->plainTextEdit->setFont(font);

    statusBar()->showMessage(QStringLiteral("就绪　——　点击工具栏最右侧的“关于”按钮查看个人信息"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCurrentUser(const QString &userName)
{
    setWindowTitle(QStringLiteral("示例 2.4  混合方式 UI 设计　——　当前用户：%1").arg(userName));
}

void MainWindow::triggerAboutAction()
{
    ui->actionAbout->trigger();
}

// ============================================================================
//  本次新增的核心功能：点击工具栏上的“关于”工具按钮，弹出 AboutDialog，
//  显示姓名、学号等个人信息。
// ============================================================================
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog(this);      // 信息来自 studentinfo.h 或 test/about.json
    dialog.exec();                 // 模态显示
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(
        this, QStringLiteral("打开文件"), QString(),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("打开文件"),
                             QStringLiteral("无法打开文件：\n%1").arg(fileName));
        return;
    }

    QTextStream stream(&file);
    ui->plainTextEdit->setPlainText(stream.readAll());
    file.close();
    statusBar()->showMessage(QStringLiteral("已打开：%1").arg(fileName), 4000);
}

void MainWindow::on_actionSave_triggered()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, QStringLiteral("保存文件"), QStringLiteral("untitled.txt"),
        QStringLiteral("文本文件 (*.txt);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("保存文件"),
                             QStringLiteral("无法写入文件：\n%1").arg(fileName));
        return;
    }

    QTextStream stream(&file);
    stream << ui->plainTextEdit->toPlainText();
    file.close();
    statusBar()->showMessage(QStringLiteral("已保存：%1").arg(fileName), 4000);
}

void MainWindow::on_actionCopy_triggered()
{
    ui->plainTextEdit->copy();
}

void MainWindow::on_actionPaste_triggered()
{
    ui->plainTextEdit->paste();
}

void MainWindow::on_actionClear_triggered()
{
    ui->plainTextEdit->clear();
}
