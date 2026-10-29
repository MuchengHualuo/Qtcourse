#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHash>
#include <QPushButton>

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

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectButtons();
    refreshDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectButtons()
{
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons) {
        const QString token = tokenTable().value(button->objectName());
        if (token.isEmpty())
            continue;
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

void MainWindow::refreshDisplay()
{
    ui->displayEdit->setText(m_engine.display());
    ui->expressionLabel->setText(m_engine.expression());
}
