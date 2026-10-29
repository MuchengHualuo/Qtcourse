#include "calculatorengine.h"

void CalculatorEngine::inputDigit(int digit)
{
    if (digit < 0 || digit > 9)
        return;

    if (m_entry == QLatin1String("0"))
        m_entry = QString::number(digit);
    else
        m_entry += QString::number(digit);
}

void CalculatorEngine::inputDecimalPoint()
{
    m_entry += QLatin1Char('.');
}

void CalculatorEngine::inputOperator(Operator op)
{
    m_firstOperand = m_entry.toDouble();
    m_pendingOperator = op;
    m_expression = m_entry + QLatin1Char(' ') + symbolOf(op);
}

void CalculatorEngine::evaluate()
{
    const QString lhsText = QString::number(m_firstOperand);
    const QString rhsText = m_entry;
    const double result = calculate(m_firstOperand, m_entry.toDouble(), m_pendingOperator);
    m_expression = lhsText + QLatin1Char(' ') + symbolOf(m_pendingOperator) + QLatin1Char(' ')
                   + rhsText + QLatin1String(" =");
    m_entry = QString::number(result);
    m_pendingOperator = Operator::None;
}

void CalculatorEngine::clearAll()
{
    m_entry = QStringLiteral("0");
    m_expression.clear();
    m_firstOperand = 0.0;
    m_pendingOperator = Operator::None;
}

void CalculatorEngine::backspace()
{
    m_entry.chop(1);
}

void CalculatorEngine::toggleSign()
{
    if (m_entry.startsWith(QLatin1Char('-')))
        m_entry.remove(0, 1);
    else
        m_entry.prepend(QLatin1Char('-'));
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

double CalculatorEngine::calculate(double lhs, double rhs, Operator op) const
{
    switch (op) {
    case Operator::Add: return lhs + rhs;
    case Operator::Subtract: return lhs - rhs;
    case Operator::Multiply: return lhs * rhs;
    case Operator::Divide: return lhs / rhs;
    case Operator::None: break;
    }
    return rhs;
}
