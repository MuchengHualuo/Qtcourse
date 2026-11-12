#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "codeeditor.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPalette>
#include <QStringConverter>
#include <QTime>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 编辑器默认外观：12pt 字体、4 个空格宽度制表位
    QFont editorFont = ui->editor->font();
    editorFont.setPointSize(12);
    ui->editor->setFont(editorFont);
    ui->editor->setTabStopDistance(4 * QFontMetricsF(editorFont).horizontalAdvance(QLatin1Char(' ')));
    ui->editor->setPlaceholderText(tr("在此输入文本，或使用“文件 → 打开”载入已有文件……"));

    // 工具栏按钮的悬停/选中样式：可选动作按下时使用浅色底，避免默认样式下的深色块
    ui->mainToolBar->setStyleSheet(
        QStringLiteral("QToolBar { border: none; padding: 2px; spacing: 2px; }"
                       "QToolButton { border: 1px solid transparent; border-radius: 4px; padding: 2px; }"
                       "QToolButton:hover { background: #eef2f8; border-color: #d7dee9; }"
                       "QToolButton:checked { background: #dce8fb; border-color: #a9c4ee; }"
                       "QToolButton:pressed { background: #d2e0f7; }"));

    initStatusBar();
    connectActions();

    // 定时自动保存：每 60 秒检查一次（选做功能）
    m_autoSaveTimer = new QTimer(this);
    m_autoSaveTimer->setInterval(60 * 1000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::autoSave);
    m_autoSaveTimer->start();

    updateActionStates();
    updateModificationState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStatusBar()
{
    m_positionLabel = new QLabel(this);
    m_infoLabel = new QLabel(this);
    m_stateLabel = new QLabel(this);

    statusBar()->addPermanentWidget(m_stateLabel);
    statusBar()->addPermanentWidget(m_infoLabel);
    statusBar()->addPermanentWidget(m_positionLabel);
    statusBar()->showMessage(tr("就绪"));

    updateCursorPosition();
}

void MainWindow::connectActions()
{
    // 文件
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    // 编辑
    connect(ui->actionUndo, &QAction::triggered, ui->editor, &QPlainTextEdit::undo);
    connect(ui->actionRedo, &QAction::triggered, ui->editor, &QPlainTextEdit::redo);
    connect(ui->actionCut, &QAction::triggered, ui->editor, &QPlainTextEdit::cut);
    connect(ui->actionCopy, &QAction::triggered, ui->editor, &QPlainTextEdit::copy);
    connect(ui->actionPaste, &QAction::triggered, ui->editor, &QPlainTextEdit::paste);
    connect(ui->actionSelectAll, &QAction::triggered, ui->editor, &QPlainTextEdit::selectAll);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::showFindDialog);
    connect(ui->actionReplace, &QAction::triggered, this, &MainWindow::showReplaceDialog);
    // 格式
    connect(ui->actionFont, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(ui->actionTextColor, &QAction::triggered, this, &MainWindow::setTextColor);
    connect(ui->actionBackgroundColor, &QAction::triggered, this, &MainWindow::setBackgroundColor);
    connect(ui->actionWordWrap, &QAction::toggled, this, &MainWindow::setWordWrap);
    // 视图
    connect(ui->actionToggleToolBar, &QAction::toggled, this, &MainWindow::setToolBarVisible);
    connect(ui->actionToggleStatusBar, &QAction::toggled, this, &MainWindow::setStatusBarVisible);
    connect(ui->actionToggleLineNumber, &QAction::toggled, this, &MainWindow::setLineNumberVisible);
    // 帮助
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAbout);

    // 编辑器的状态变化驱动动作使能与状态栏刷新
    connect(ui->editor->document(), &QTextDocument::modificationChanged,
            this, &MainWindow::updateModificationState);
    connect(ui->editor, &QPlainTextEdit::cursorPositionChanged,
            this, &MainWindow::updateCursorPosition);
    connect(ui->editor, &QPlainTextEdit::textChanged,
            this, &MainWindow::updateCursorPosition);
    connect(ui->editor, &QPlainTextEdit::copyAvailable,
            this, &MainWindow::updateActionStates);
    connect(ui->editor, &QPlainTextEdit::undoAvailable,
            this, &MainWindow::updateActionStates);
    connect(ui->editor, &QPlainTextEdit::redoAvailable,
            this, &MainWindow::updateActionStates);
    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &MainWindow::updateActionStates);
}

// ---------------------------------------------------------------- 状态刷新

void MainWindow::updateCursorPosition()
{
    const QTextCursor cursor = ui->editor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int column = cursor.columnNumber() + 1;
    m_positionLabel->setText(tr("行 %1，列 %2").arg(line).arg(column));
    m_infoLabel->setText(tr("共 %1 行 / %2 字符")
                             .arg(ui->editor->blockCount())
                             .arg(qMax(0, ui->editor->document()->characterCount() - 1)));
}

void MainWindow::updateModificationState()
{
    const bool modified = ui->editor->document()->isModified();
    m_stateLabel->setText(modified ? tr("已修改") : tr("未修改"));
    updateWindowTitle();
    updateActionStates();
}

void MainWindow::updateActionStates()
{
    const bool hasSelection = ui->editor->textCursor().hasSelection();
    ui->actionCopy->setEnabled(hasSelection);
    ui->actionCut->setEnabled(hasSelection);
    ui->actionUndo->setEnabled(ui->editor->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->editor->document()->isRedoAvailable());

    // 只有剪贴板中存在文本时才允许粘贴
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    ui->actionPaste->setEnabled(mime != nullptr && mime->hasText());
    ui->actionSelectAll->setEnabled(!ui->editor->document()->isEmpty());
}

void MainWindow::updateWindowTitle()
{
    const QString name = m_currentFile.isEmpty() ? tr("无标题")
                                                 : QFileInfo(m_currentFile).fileName();
    const QString mark = ui->editor->document()->isModified() ? QStringLiteral("*") : QString();
    setWindowTitle(tr("%1%2 - 简单文本编辑器").arg(mark, name));
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    m_currentFile = fileName;
    ui->editor->document()->setModified(false);
    updateModificationState();
}

// ---------------------------------------------------------------- 文件操作

void MainWindow::newFile()
{
    if (!maybeSave())
        return;

    ui->editor->clear();
    setCurrentFile(QString());
    statusBar()->showMessage(tr("已新建空白文档"), 3000);
}

void MainWindow::openFile()
{
    if (!maybeSave())
        return;

    const QString fileName = QFileDialog::getOpenFileName(
        this, tr("打开文件"), QDir::homePath(),
        tr("文本文件 (*.txt *.md *.cpp *.h *.log);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}

bool MainWindow::loadFile(const QString &fileName)
{
    const QFileInfo info(fileName);
    // 边界情况一：文件不存在
    if (!info.exists() || !info.isFile()) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("文件不存在：\n%1").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }
    // 边界情况二：没有读取权限
    if (!info.isReadable()) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("没有读取该文件的权限：\n%1").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }
    // 边界情况三：文件过大时给出提示，由用户决定是否继续
    if (info.size() > 20 * 1024 * 1024) {
        const auto ret = QMessageBox::question(
            this, tr("文件较大"),
            tr("文件大小为 %1 MB，打开后可能影响响应速度，是否继续？")
                .arg(QString::number(info.size() / 1024.0 / 1024.0, 'f', 1)),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes)
            return false;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("无法打开文件：\n%1\n%2")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }
    const QByteArray data = file.readAll();
    file.close();

    // 边界情况四：二进制文件（含 '\0'）不能按文本打开
    if (data.contains('\0')) {
        QMessageBox::warning(this, tr("打开失败"),
                             tr("该文件不是纯文本文件，无法作为文本打开：\n%1")
                                 .arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    QString text = QString::fromUtf8(data);
    if (!text.isEmpty() && text.at(0) == QChar(0xFEFF))   // 去掉 UTF-8 BOM
        text.remove(0, 1);

    ui->editor->setPlainText(text);
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("已打开 %1").arg(QDir::toNativeSeparators(fileName)), 3000);
    return true;
}

bool MainWindow::saveFile()
{
    if (m_currentFile.isEmpty())
        return saveFileAs();
    return saveToFile(m_currentFile);
}

bool MainWindow::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("另存为"),
        m_currentFile.isEmpty() ? QDir::homePath() + QStringLiteral("/无标题.txt") : m_currentFile,
        tr("文本文件 (*.txt);;Markdown 文件 (*.md);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return false;
    // 未填写扩展名时自动补上 .txt
    if (QFileInfo(fileName).suffix().isEmpty())
        fileName += QStringLiteral(".txt");
    return saveToFile(fileName);
}

bool MainWindow::saveToFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::warning(this, tr("保存失败"),
                             tr("无法写入文件：\n%1\n%2")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << ui->editor->toPlainText();
    out.flush();
    const bool ok = (out.status() == QTextStream::Ok);
    file.close();

    if (!ok) {
        QMessageBox::warning(this, tr("保存失败"),
                             tr("写入文件时发生错误：\n%1").arg(QDir::toNativeSeparators(fileName)));
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("已保存到 %1").arg(QDir::toNativeSeparators(fileName)), 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!ui->editor->document()->isModified())
        return true;

    const QString name = m_currentFile.isEmpty() ? tr("无标题")
                                                 : QFileInfo(m_currentFile).fileName();
    const auto ret = QMessageBox::warning(
        this, tr("简单文本编辑器"),
        tr("文档“%1”已被修改，是否保存更改？").arg(name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save);

    switch (ret) {
    case QMessageBox::Save:
        return saveFile();
    case QMessageBox::Discard:
        return true;
    default:
        return false;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

// ---------------------------------------------------------------- 格式设置

void MainWindow::chooseFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, ui->editor->font(), this, tr("选择字体"));
    if (!ok)
        return;

    QTextCursor cursor = ui->editor->textCursor();
    if (cursor.hasSelection()) {
        QTextCharFormat format;
        format.setFont(font);
        cursor.mergeCharFormat(format);
        ui->editor->mergeCurrentCharFormat(format);
        statusBar()->showMessage(tr("已设置选中文本的字体"), 3000);
    } else {
        // 没有选中文本时设置整个编辑器的字体
        ui->editor->setFont(font);
        statusBar()->showMessage(tr("已设置编辑器字体"), 3000);
    }
}

void MainWindow::setTextColor()
{
    const QColor color = QColorDialog::getColor(
        ui->editor->currentCharFormat().foreground().color(), this, tr("选择字体颜色"));
    if (!color.isValid())
        return;

    QTextCharFormat format;
    format.setForeground(color);

    QTextCursor cursor = ui->editor->textCursor();
    if (cursor.hasSelection()) {
        cursor.mergeCharFormat(format);
        ui->editor->mergeCurrentCharFormat(format);
        statusBar()->showMessage(tr("已设置选中文本的颜色"), 3000);
    } else {
        // 没有选中文本时对整个文档设置颜色
        QTextCursor all(ui->editor->document());
        all.select(QTextCursor::Document);
        all.mergeCharFormat(format);
        statusBar()->showMessage(tr("已设置全文的字体颜色"), 3000);
    }
}

void MainWindow::setBackgroundColor()
{
    const QColor color = QColorDialog::getColor(ui->editor->palette().color(QPalette::Base),
                                                this, tr("选择背景色"));
    if (!color.isValid())
        return;

    QPalette palette = ui->editor->palette();
    palette.setColor(QPalette::Base, color);
    // 深色背景时自动改用白色默认文字，保证可读性
    palette.setColor(QPalette::Text, color.lightness() < 128 ? Qt::white : Qt::black);
    ui->editor->setPalette(palette);
    statusBar()->showMessage(tr("已设置背景色"), 3000);
}

void MainWindow::setWordWrap(bool enabled)
{
    ui->editor->setLineWrapMode(enabled ? QPlainTextEdit::WidgetWidth
                                        : QPlainTextEdit::NoWrap);
    statusBar()->showMessage(enabled ? tr("已打开自动换行") : tr("已关闭自动换行"), 3000);
}

// ---------------------------------------------------------------- 视图

void MainWindow::setToolBarVisible(bool visible)
{
    ui->mainToolBar->setVisible(visible);
}

void MainWindow::setStatusBarVisible(bool visible)
{
    statusBar()->setVisible(visible);
}

void MainWindow::setLineNumberVisible(bool visible)
{
    ui->editor->setLineNumberVisible(visible);
    statusBar()->showMessage(visible ? tr("已显示行号") : tr("已隐藏行号"), 3000);
}

// ---------------------------------------------------------------- 帮助与对话框

void MainWindow::showAbout()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::showFindDialog()
{
    if (!m_findDialog) {
        m_findDialog = new FindDialog(this);
        connect(m_findDialog, &FindDialog::findNextRequested, this, &MainWindow::findNext);
    }

    // 用编辑器中选中的单行文本作为默认查找内容
    const QString selected = ui->editor->textCursor().selectedText();
    if (!selected.isEmpty() && !selected.contains(QChar::ParagraphSeparator))
        m_findDialog->setFindText(selected);

    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}

void MainWindow::showReplaceDialog()
{
    if (!m_replaceDialog) {
        m_replaceDialog = new ReplaceDialog(this);
        connect(m_replaceDialog, &ReplaceDialog::findNextRequested, this, &MainWindow::findNext);
        connect(m_replaceDialog, &ReplaceDialog::replaceRequested, this, &MainWindow::replaceOne);
        connect(m_replaceDialog, &ReplaceDialog::replaceAllRequested, this, &MainWindow::replaceAll);
    }

    const QString selected = ui->editor->textCursor().selectedText();
    if (!selected.isEmpty() && !selected.contains(QChar::ParagraphSeparator))
        m_replaceDialog->setFindText(selected);

    m_replaceDialog->show();
    m_replaceDialog->raise();
    m_replaceDialog->activateWindow();
}

// ---------------------------------------------------------------- 查找与替换

bool MainWindow::findFrom(const QString &text, bool caseSensitive, bool &wrapped)
{
    wrapped = false;
    if (text.isEmpty())
        return false;

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor found = ui->editor->document()->find(text, ui->editor->textCursor(), flags);
    if (found.isNull()) {
        // 到达文档末尾后从头继续查找，实现连续查找
        QTextCursor fromStart(ui->editor->document());
        found = ui->editor->document()->find(text, fromStart, flags);
        if (!found.isNull())
            wrapped = true;
    }

    if (found.isNull())
        return false;

    ui->editor->setTextCursor(found);
    ui->editor->ensureCursorVisible();
    return true;
}

void MainWindow::findNext(const QString &text, bool caseSensitive)
{
    if (text.isEmpty())
        return;

    bool wrapped = false;
    QString message;
    if (findFrom(text, caseSensitive, wrapped)) {
        message = wrapped ? tr("已从头继续查找") : tr("已找到");
        statusBar()->showMessage(message, 3000);
    } else {
        message = tr("未找到“%1”").arg(text);
        statusBar()->showMessage(message, 4000);
    }

    if (m_findDialog)
        m_findDialog->showResult(message);
    if (m_replaceDialog)
        m_replaceDialog->showResult(message);
}

void MainWindow::replaceOne(const QString &text, const QString &replacement, bool caseSensitive)
{
    QTextCursor cursor = ui->editor->textCursor();
    const QString selected = cursor.selectedText();
    const bool matched = cursor.hasSelection()
                         && (caseSensitive ? selected == text
                                           : selected.compare(text, Qt::CaseInsensitive) == 0);

    if (matched) {
        cursor.insertText(replacement);
        ui->editor->setTextCursor(cursor);
    }
    // 替换完成（或当前选区不匹配）后自动定位到下一处
    findNext(text, caseSensitive);
}

void MainWindow::replaceAll(const QString &text, const QString &replacement, bool caseSensitive)
{
    if (text.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    QTextCursor found = ui->editor->document()->find(text, 0, flags);
    if (found.isNull()) {
        const QString message = tr("未找到“%1”").arg(text);
        statusBar()->showMessage(message, 4000);
        if (m_replaceDialog)
            m_replaceDialog->showResult(message);
        return;
    }

    int count = 0;
    QTextCursor editCursor(ui->editor->document());
    editCursor.beginEditBlock();   // 全部替换合并为一次编辑，可一次撤销
    while (!found.isNull()) {
        found.insertText(replacement);
        ++count;
        // 从替换位置之后继续查找：即使替换文本包含查找词也不会重复匹配
        found = ui->editor->document()->find(text, found, flags);
    }
    editCursor.endEditBlock();

    const QString message = tr("共替换 %1 处").arg(count);
    statusBar()->showMessage(message, 4000);
    if (m_replaceDialog)
        m_replaceDialog->showResult(message);
}

// ---------------------------------------------------------------- 自动保存

void MainWindow::autoSave()
{
    if (!ui->editor->document()->isModified())
        return;
    if (m_currentFile.isEmpty())   // 从未保存过的新文件没有路径，跳过自动保存
        return;

    if (saveToFile(m_currentFile))
        statusBar()->showMessage(tr("已自动保存 %1").arg(QTime::currentTime().toString("HH:mm:ss")), 3000);
}
