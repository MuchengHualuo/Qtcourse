#ifndef CALCULATORENGINE_H
#define CALCULATORENGINE_H

#include <QString>

/**
 * @brief 计算器核心逻辑，不依赖任何界面控件，便于单元测试与复用。
 *
 * 状态机由以下几个成员构成：
 *  - m_firstOperand     第一个操作数（连续运算时同时充当中间结果）；
 *  - m_pendingOperator  已按下、尚未执行的运算符；
 *  - m_entry            当前正在输入（或显示）的第二个操作数；
 *  - m_waitingForEntry  为 true 时表示下一次数字输入应重新开始一个操作数
 *                       （刚按完运算符，或刚得到计算结果）；
 *  - m_errorState       除零、溢出等错误状态，错误时只接受“清除/数字”恢复。
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

    QString display() const;
    QString expression() const { return m_expression; }
    bool hasError() const { return m_errorState; }

private:
    static QString symbolOf(Operator op);
    static int digitCount(const QString &text);

    bool compute(double lhs, double rhs, Operator op, double &result);
    void enterError(const QString &message);
    void refreshExpression();

    QString m_entry = QStringLiteral("0");
    QString m_expression;
    double m_firstOperand = 0.0;
    Operator m_pendingOperator = Operator::None;
    bool m_waitingForEntry = true;
    bool m_errorState = false;
    QString m_errorText;

    // 连按“=”时重复使用的运算符与第二个操作数
    Operator m_repeatOperator = Operator::None;
    double m_repeatOperand = 0.0;
};

#endif // CALCULATORENGINE_H
