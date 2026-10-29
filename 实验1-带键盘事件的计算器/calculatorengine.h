#ifndef CALCULATORENGINE_H
#define CALCULATORENGINE_H

#include <QString>

/**
 * @brief 计算器核心逻辑，不依赖任何界面控件，便于单元测试。
 */
class CalculatorEngine
{
public:
    enum class Operator { None, Add, Subtract, Multiply, Divide };

    void inputDigit(int digit);
    void inputDecimalPoint();
    void inputOperator(Operator op);
    void evaluate();
    void clearAll();
    void backspace();
    void toggleSign();

    QString display() const { return m_entry; }
    QString expression() const { return m_expression; }

private:
    static QString symbolOf(Operator op);
    double calculate(double lhs, double rhs, Operator op) const;

    QString m_entry = QStringLiteral("0");
    QString m_expression;
    double m_firstOperand = 0.0;
    Operator m_pendingOperator = Operator::None;
};

#endif // CALCULATORENGINE_H
