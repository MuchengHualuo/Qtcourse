#include "calculatorengine.h"

#include <cmath>

namespace {

/// 允许输入的最大有效数字位数，防止显示区被无限撑长。
constexpr int kMaxDigits = 15;

/// 把 double 转成适合显示的数字串：整数不带小数点，小数最多 12 位有效数字。
QString formatNumber(double value)
{
    if (!std::isfinite(value))
        return QStringLiteral("溢出");

    // 抹掉 0.1 + 0.2 这类二进制浮点尾差，同时把 -0 归一为 0
    const double rounded = std::round(value * 1e12) / 1e12;
    if (std::fabs(rounded) < 1e15 && std::floor(rounded) == rounded)
        return QString::number(static_cast<qlonglong>(rounded));

    return QString::number(rounded, 'g', 12);
}

} // namespace

void CalculatorEngine::inputDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    if (m_errorState)
        clearAll();

    if (m_waitingForEntry) {
        // 新操作数的第一位：直接替换当前显示，而不是追加
        // （先按“±”再输入数字时保留负号）
        const bool negative = (m_entry == QLatin1String("-0"));
        m_entry = (negative ? QStringLiteral("-") : QString()) + QString::number(digit);
        m_waitingForEntry = false;
        if (m_pendingOperator == Operator::None)
            m_expression.clear(); // 计算结果后重新输入，清掉上一轮的算式提示
    } else if (m_entry == QLatin1String("0")) {
        m_entry = QString::number(digit);
    } else if (m_entry == QLatin1String("-0")) {
        m_entry = QLatin1Char('-') + QString::number(digit);
    } else if (digitCount(m_entry) < kMaxDigits) {
        m_entry += QString::number(digit);
    }

    refreshExpression();
}

void CalculatorEngine::inputDecimalPoint()
{
    if (m_errorState)
        clearAll();

    if (m_waitingForEntry) {
        m_entry = QStringLiteral("0.");
        m_waitingForEntry = false;
        if (m_pendingOperator == Operator::None)
            m_expression.clear();
    } else if (!m_entry.contains(QLatin1Char('.'))) {
        // 同一个操作数只允许出现一个小数点
        m_entry += QLatin1Char('.');
    }

    refreshExpression();
}

void CalculatorEngine::inputOperator(Operator op)
{
    if (op == Operator::None || m_errorState)
        return; // 错误状态下必须先清除，避免把错误结果带入新算式

    if (m_pendingOperator != Operator::None) {
        if (m_waitingForEntry) {
            // 连续按下运算符：只替换运算符，不产生额外计算
            m_pendingOperator = op;
            refreshExpression();
            return;
        }

        // 连续运算：先把上一段算式结算成中间结果
        double intermediate = 0.0;
        if (!compute(m_firstOperand, m_entry.toDouble(), m_pendingOperator, intermediate))
            return;
        m_entry = formatNumber(intermediate);
        m_firstOperand = intermediate;
    } else {
        m_firstOperand = m_entry.toDouble();
    }

    m_pendingOperator = op;
    m_waitingForEntry = true;
    m_repeatOperator = Operator::None;
    refreshExpression();
}

void CalculatorEngine::evaluate()
{
    if (m_errorState)
        return;

    if (m_pendingOperator == Operator::None) {
        // 没有待执行的运算时，“=”用于重复上一次运算（例如 2 + 3 = = ）
        if (m_repeatOperator == Operator::None)
            return;

        const double lhs = m_entry.toDouble();
        double result = 0.0;
        if (!compute(lhs, m_repeatOperand, m_repeatOperator, result))
            return;
        m_expression = formatNumber(lhs) + QLatin1Char(' ') + symbolOf(m_repeatOperator)
                       + QLatin1Char(' ') + formatNumber(m_repeatOperand) + QLatin1String(" =");
        m_entry = formatNumber(result);
        m_waitingForEntry = true;
        return;
    }

    // 只按了“5 + =”的情况下，把第一个操作数当作第二个操作数（与常见计算器一致）
    const double lhs = m_firstOperand;
    const double rhs = m_waitingForEntry ? m_firstOperand : m_entry.toDouble();
    const QString rhsText = m_waitingForEntry ? formatNumber(m_firstOperand) : m_entry;
    const QString expressionText = formatNumber(lhs) + QLatin1Char(' ')
                                   + symbolOf(m_pendingOperator) + QLatin1Char(' ') + rhsText
                                   + QLatin1String(" =");

    double result = 0.0;
    if (!compute(lhs, rhs, m_pendingOperator, result)) {
        m_expression = expressionText; // 保留算式，便于定位出错的输入
        return;
    }

    m_expression = expressionText;
    m_repeatOperator = m_pendingOperator;
    m_repeatOperand = rhs;
    m_entry = formatNumber(result);
    m_firstOperand = result;
    m_pendingOperator = Operator::None;
    m_waitingForEntry = true;
}

void CalculatorEngine::clearAll()
{
    m_entry = QStringLiteral("0");
    m_expression.clear();
    m_firstOperand = 0.0;
    m_pendingOperator = Operator::None;
    m_waitingForEntry = true;
    m_errorState = false;
    m_errorText.clear();
    m_repeatOperator = Operator::None;
    m_repeatOperand = 0.0;
}

void CalculatorEngine::backspace()
{
    if (m_errorState) {
        clearAll(); // 错误状态下退格视为“清除”
        return;
    }

    if (m_waitingForEntry)
        return; // 刚按完运算符或刚得到结果时，没有可删除的输入

    m_entry.chop(1);
    if (m_entry.isEmpty() || m_entry == QLatin1String("-"))
        m_entry = QStringLiteral("0");

    refreshExpression();
}

void CalculatorEngine::toggleSign()
{
    if (m_errorState)
        clearAll();

    if (m_entry.startsWith(QLatin1Char('-')))
        m_entry.remove(0, 1);
    else
        m_entry.prepend(QLatin1Char('-'));

    refreshExpression();
}

QString CalculatorEngine::display() const
{
    return m_errorState ? m_errorText : m_entry;
}

QString CalculatorEngine::symbolOf(Operator op)
{
    switch (op) {
    case Operator::Add: return QStringLiteral("+");
    case Operator::Subtract: return QStringLiteral("-");
    case Operator::Multiply: return QStringLiteral("×");
    case Operator::Divide: return QStringLiteral("÷");
    case Operator::None: break;
    }
    return QString();
}

int CalculatorEngine::digitCount(const QString &text)
{
    int count = 0;
    for (const QChar &c : text) {
        if (c.isDigit())
            ++count;
    }
    return count;
}

bool CalculatorEngine::compute(double lhs, double rhs, Operator op, double &result)
{
    switch (op) {
    case Operator::Add:
        result = lhs + rhs;
        break;
    case Operator::Subtract:
        result = lhs - rhs;
        break;
    case Operator::Multiply:
        result = lhs * rhs;
        break;
    case Operator::Divide:
        if (rhs == 0.0) {
            enterError(QStringLiteral("除数不能为 0"));
            return false;
        }
        result = lhs / rhs;
        break;
    case Operator::None:
        result = rhs;
        break;
    }

    if (!std::isfinite(result)) {
        enterError(QStringLiteral("数值超出范围"));
        return false;
    }
    return true;
}

void CalculatorEngine::enterError(const QString &message)
{
    m_errorState = true;
    m_errorText = message;
    m_pendingOperator = Operator::None;
    m_repeatOperator = Operator::None;
    m_waitingForEntry = true;
}

void CalculatorEngine::refreshExpression()
{
    if (m_pendingOperator == Operator::None)
        return;

    m_expression = formatNumber(m_firstOperand) + QLatin1Char(' ') + symbolOf(m_pendingOperator);
    if (!m_waitingForEntry)
        m_expression += QLatin1Char(' ') + m_entry;
}
