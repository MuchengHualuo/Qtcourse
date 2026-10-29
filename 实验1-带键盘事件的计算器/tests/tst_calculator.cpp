#include "calculatorengine.h"

#include <QApplication>
#include <QtTest>

/**
 * @brief CalculatorEngine 单元测试：覆盖基本运算、小数、退格、清除以及各类异常输入。
 */
class CalculatorEngineTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup() {}

    void initialState();
    void digitsAndLeadingZero();
    void decimalInput();
    void repeatedDecimalPointIsIgnored();
    void leadingDecimalPoint();
    void basicOperations_data();
    void basicOperations();
    void chainedOperations();
    void repeatedOperatorReplacesPrevious();
    void divideByZero();
    void recoverFromError();
    void clearResetsEverything();
    void continueInputAfterResult();
    void continueWithOperatorAfterResult();
    void backspaceWhileTyping();
    void backspaceAfterResult();
    void repeatedEquals();
    void floatingPointTail();
    void divisionPrecision();
    void digitLimit();
    void signToggle();

private:
    void type(const QString &keys);

    CalculatorEngine m_engine;
};

void CalculatorEngineTest::init()
{
    m_engine = CalculatorEngine();
}

void CalculatorEngineTest::type(const QString &keys)
{
    for (const QChar &key : keys) {
        if (key.isDigit())
            m_engine.inputDigit(key.digitValue());
        else if (key == QLatin1Char('.'))
            m_engine.inputDecimalPoint();
        else if (key == QLatin1Char('+'))
            m_engine.inputOperator(CalculatorEngine::Operator::Add);
        else if (key == QLatin1Char('-'))
            m_engine.inputOperator(CalculatorEngine::Operator::Subtract);
        else if (key == QLatin1Char('*'))
            m_engine.inputOperator(CalculatorEngine::Operator::Multiply);
        else if (key == QLatin1Char('/'))
            m_engine.inputOperator(CalculatorEngine::Operator::Divide);
        else if (key == QLatin1Char('='))
            m_engine.evaluate();
        else if (key == QLatin1Char('C'))
            m_engine.clearAll();
        else if (key == QLatin1Char('<'))
            m_engine.backspace();
        else if (key == QLatin1Char('~'))
            m_engine.toggleSign();
        else
            QFAIL("unsupported test key");
    }
}

void CalculatorEngineTest::initialState()
{
    QCOMPARE(m_engine.display(), QStringLiteral("0"));
    QCOMPARE(m_engine.expression(), QString());
    QVERIFY(!m_engine.hasError());
}

void CalculatorEngineTest::digitsAndLeadingZero()
{
    type(QStringLiteral("0005"));
    QCOMPARE(m_engine.display(), QStringLiteral("5"));
    QVERIFY(!m_engine.hasError());
}

void CalculatorEngineTest::decimalInput()
{
    type(QStringLiteral("1.23"));
    QCOMPARE(m_engine.display(), QStringLiteral("1.23"));
}

void CalculatorEngineTest::repeatedDecimalPointIsIgnored()
{
    type(QStringLiteral("1.2.3"));
    QCOMPARE(m_engine.display(), QStringLiteral("1.23"));
}

void CalculatorEngineTest::leadingDecimalPoint()
{
    type(QStringLiteral(".5"));
    QCOMPARE(m_engine.display(), QStringLiteral("0.5"));
}

void CalculatorEngineTest::basicOperations_data()
{
    QTest::addColumn<QString>("keys");
    QTest::addColumn<QString>("expected");

    QTest::newRow("12+3") << QStringLiteral("12+3=") << QStringLiteral("15");
    QTest::newRow("12-20") << QStringLiteral("12-20=") << QStringLiteral("-8");
    QTest::newRow("1.5*4") << QStringLiteral("1.5*4=") << QStringLiteral("6");
    QTest::newRow("9/4") << QStringLiteral("9/4=") << QStringLiteral("2.25");
    QTest::newRow("0.1+0.2") << QStringLiteral(".1+.2=") << QStringLiteral("0.3");
}

void CalculatorEngineTest::basicOperations()
{
    QFETCH(QString, keys);
    QFETCH(QString, expected);
    type(keys);
    QCOMPARE(m_engine.display(), expected);
    QVERIFY(!m_engine.hasError());
}

void CalculatorEngineTest::chainedOperations()
{
    type(QStringLiteral("2+3+4="));
    // 按下第二个“+”时应先把 2+3 结算为 5，再参与后续运算
    QCOMPARE(m_engine.display(), QStringLiteral("9"));

    type(QStringLiteral("C2+3*4="));
    QCOMPARE(m_engine.display(), QStringLiteral("20")); // 顺序执行： (2+3)*4
}

void CalculatorEngineTest::repeatedOperatorReplacesPrevious()
{
    type(QStringLiteral("5+*3="));
    QCOMPARE(m_engine.display(), QStringLiteral("15"));
    QVERIFY(!m_engine.hasError());
}

void CalculatorEngineTest::divideByZero()
{
    type(QStringLiteral("8/0="));
    QVERIFY(m_engine.hasError());
    QCOMPARE(m_engine.display(), QStringLiteral("除数不能为 0"));
    QCOMPARE(m_engine.expression(), QStringLiteral("8 ÷ 0 ="));
}

void CalculatorEngineTest::recoverFromError()
{
    type(QStringLiteral("8/0="));
    type(QStringLiteral("5"));
    QVERIFY(!m_engine.hasError());
    QCOMPARE(m_engine.display(), QStringLiteral("5"));

    type(QStringLiteral("8/0="));
    type(QStringLiteral("C"));
    QCOMPARE(m_engine.display(), QStringLiteral("0"));
    QVERIFY(!m_engine.hasError());
}

void CalculatorEngineTest::clearResetsEverything()
{
    type(QStringLiteral("12+34"));
    QCOMPARE(m_engine.expression(), QStringLiteral("12 + 34"));
    type(QStringLiteral("C"));
    QCOMPARE(m_engine.display(), QStringLiteral("0"));
    QCOMPARE(m_engine.expression(), QString());
    type(QStringLiteral("7+1="));
    QCOMPARE(m_engine.display(), QStringLiteral("8"));
}

void CalculatorEngineTest::continueInputAfterResult()
{
    type(QStringLiteral("2+3=4"));
    QCOMPARE(m_engine.display(), QStringLiteral("4"));
    type(QStringLiteral("5"));
    QCOMPARE(m_engine.display(), QStringLiteral("45"));
}

void CalculatorEngineTest::continueWithOperatorAfterResult()
{
    type(QStringLiteral("2+3=+1="));
    QCOMPARE(m_engine.display(), QStringLiteral("6"));
}

void CalculatorEngineTest::backspaceWhileTyping()
{
    type(QStringLiteral("123<<"));
    QCOMPARE(m_engine.display(), QStringLiteral("1"));
    type(QStringLiteral("<<"));
    QCOMPARE(m_engine.display(), QStringLiteral("0"));
}

void CalculatorEngineTest::backspaceAfterResult()
{
    type(QStringLiteral("2+3=<"));
    QCOMPARE(m_engine.display(), QStringLiteral("5")); // 结果不可退格

    type(QStringLiteral("C12+34<"));
    QCOMPARE(m_engine.display(), QStringLiteral("3"));
    QCOMPARE(m_engine.expression(), QStringLiteral("12 + 3"));
}

void CalculatorEngineTest::repeatedEquals()
{
    type(QStringLiteral("2+3="));
    QCOMPARE(m_engine.display(), QStringLiteral("5"));
    type(QStringLiteral("="));
    QCOMPARE(m_engine.display(), QStringLiteral("8"));
    type(QStringLiteral("="));
    QCOMPARE(m_engine.display(), QStringLiteral("11"));
}

void CalculatorEngineTest::floatingPointTail()
{
    type(QStringLiteral(".1+.2="));
    QCOMPARE(m_engine.display(), QStringLiteral("0.3"));

    type(QStringLiteral("C1.1*1.1="));
    QCOMPARE(m_engine.display(), QStringLiteral("1.21"));
}

void CalculatorEngineTest::divisionPrecision()
{
    type(QStringLiteral("1/3="));
    QCOMPARE(m_engine.display(), QStringLiteral("0.333333333333"));
}

void CalculatorEngineTest::digitLimit()
{
    type(QStringLiteral("123456789012345678"));
    QCOMPARE(m_engine.display(), QStringLiteral("123456789012345"));
}

void CalculatorEngineTest::signToggle()
{
    type(QStringLiteral("5~"));
    QCOMPARE(m_engine.display(), QStringLiteral("-5"));
    type(QStringLiteral("~"));
    QCOMPARE(m_engine.display(), QStringLiteral("5"));
    type(QStringLiteral("C~5"));
    QCOMPARE(m_engine.display(), QStringLiteral("-5"));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    int status = 0;
    CalculatorEngineTest engineTest;
    status |= QTest::qExec(&engineTest, argc, argv);
    return status;
}

#include "tst_calculator.moc"
