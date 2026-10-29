#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QKeyEvent>
#include <QPushButton>
#include <QStyle>

namespace {

/// 按钮对象名 → 输入记号。鼠标与键盘最终都转换成同一套记号。
const QHash<QString, QString> &tokenTable()
{
    static const QHash<QString, QString> table = {
        {QStringLiteral("btn0"), QStringLiteral("0")},
        {QStringLiteral("btn1"), QStringLiteral("1")},
        {QStringLiteral("btn2"), QStringLiteral("2")},
        {QStringLiteral("btn3"), QStringLiteral("3")},
        {QStringLiteral("btn4"), QStringLiteral("4")},
        {QStringLiteral("btn5"), QStringLiteral("5")},
        {QStringLiteral("btn6"), QStringLiteral("6")},
        {QStringLiteral("btn7"), QStringLiteral("7")},
        {QStringLiteral("btn8"), QStringLiteral("8")},
        {QStringLiteral("btn9"), QStringLiteral("9")},
        {QStringLiteral("btnDot"), QStringLiteral(".")},
        {QStringLiteral("btnAdd"), QStringLiteral("+")},
        {QStringLiteral("btnSubtract"), QStringLiteral("-")},
        {QStringLiteral("btnMultiply"), QStringLiteral("*")},
        {QStringLiteral("btnDivide"), QStringLiteral("/")},
        {QStringLiteral("btnEquals"), QStringLiteral("=")},
        {QStringLiteral("btnClear"), QStringLiteral("clear")},
        {QStringLiteral("btnBackspace"), QStringLiteral("backspace")},
        {QStringLiteral("btnSign"), QStringLiteral("negate")},
    };
    return table;
}

/// 键盘按键 → 输入记号；返回空串表示该按键不参与计算。
QString tokenForKey(int key)
{
    if (key >= Qt::Key_0 && key <= Qt::Key_9)
        return QString::number(key - Qt::Key_0);

    switch (key) {
    case Qt::Key_Period:
    case Qt::Key_Comma: // 小键盘的“,”在小数点位置
        return QStringLiteral(".");
    case Qt::Key_Plus:
        return QStringLiteral("+");
    case Qt::Key_Minus:
        return QStringLiteral("-");
    case Qt::Key_Asterisk:
    case Qt::Key_multiply:
        return QStringLiteral("*");
    case Qt::Key_Slash:
    case Qt::Key_division:
        return QStringLiteral("/");
    case Qt::Key_Equal:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return QStringLiteral("=");
    case Qt::Key_Backspace:
        return QStringLiteral("backspace");
    case Qt::Key_Escape:
    case Qt::Key_Delete:
    case Qt::Key_C:
        return QStringLiteral("clear");
    default:
        return QString();
    }
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    applyStyleSheet();
    configureWidgets();
    connectButtons();
    refreshDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::applyStyleSheet()
{
    QFile styleFile(QStringLiteral(":/style.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
}

void MainWindow::configureWidgets()
{
    // 数字区按钮随窗口高度一起拉伸，窗口放大时不会留下大片空白
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        // 按钮不抢焦点，保证键盘事件始终由主窗口处理
        button->setFocusPolicy(Qt::NoFocus);
    }
    ui->displayEdit->setFocusPolicy(Qt::NoFocus);
}

void MainWindow::connectButtons()
{
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        const QString token = tokenTable().value(button->objectName());
        if (token.isEmpty())
            continue;
        m_buttonByToken.insert(token, button);
        connect(button, &QPushButton::clicked, this, [this, token] { handleToken(token); });
    }
}

void MainWindow::handleToken(const QString &token)
{
    if (token.size() == 1 && token.at(0).isDigit()) {
        m_engine.inputDigit(token.toInt());
    } else if (token == QLatin1String(".")) {
        m_engine.inputDecimalPoint();
    } else if (token == QLatin1String("+")) {
        m_engine.inputOperator(CalculatorEngine::Operator::Add);
    } else if (token == QLatin1String("-")) {
        m_engine.inputOperator(CalculatorEngine::Operator::Subtract);
    } else if (token == QLatin1String("*")) {
        m_engine.inputOperator(CalculatorEngine::Operator::Multiply);
    } else if (token == QLatin1String("/")) {
        m_engine.inputOperator(CalculatorEngine::Operator::Divide);
    } else if (token == QLatin1String("=")) {
        m_engine.evaluate();
    } else if (token == QLatin1String("clear")) {
        m_engine.clearAll();
    } else if (token == QLatin1String("backspace")) {
        m_engine.backspace();
    } else if (token == QLatin1String("negate")) {
        m_engine.toggleSign();
    }

    refreshDisplay();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 带 Ctrl/Alt/Win 的组合键留给系统或窗口快捷键
    if (event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    const QString token = tokenForKey(event->key());
    if (token.isEmpty()) {
        QMainWindow::keyPressEvent(event);
        return;
    }

    handleToken(token);
    if (!event->isAutoRepeat())
        setButtonDown(token, true); // 键盘输入时同步高亮对应按钮
    event->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    const QString token = tokenForKey(event->key());
    if (token.isEmpty()) {
        QMainWindow::keyReleaseEvent(event);
        return;
    }

    setButtonDown(token, false);
    event->accept();
}

void MainWindow::setButtonDown(const QString &token, bool pressed)
{
    if (QPushButton *button = m_buttonByToken.value(token))
        button->setDown(pressed);
}

void MainWindow::refreshDisplay()
{
    ui->displayEdit->setText(m_engine.display());
    ui->expressionLabel->setText(m_engine.expression());

    // 错误提示（如“除数不能为 0”）使用样式表中的 error 状态显示
    const bool error = m_engine.hasError();
    if (ui->displayEdit->property("error").toBool() != error) {
        ui->displayEdit->setProperty("error", error);
        ui->displayEdit->style()->unpolish(ui->displayEdit);
        ui->displayEdit->style()->polish(ui->displayEdit);
    }
}
