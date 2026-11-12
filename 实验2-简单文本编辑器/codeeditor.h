#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

/**
 * CodeEditor：从 QPlainTextEdit 派生的编辑器控件。
 * 在左侧额外维护一个行号区域，并高亮当前行。
 * 该类在 Qt Designer 中通过“提升（promote）”替换原来的 QPlainTextEdit。
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    // 行号区域需要的宽度（随总行数位数变化）
    int lineNumberAreaWidth() const;

    // 绘制行号（由行号区域控件的 paintEvent 调用）
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    bool isLineNumberVisible() const { return m_lineNumberVisible; }

public slots:
    void setLineNumberVisible(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    QWidget *m_lineNumberArea = nullptr;
    bool m_lineNumberVisible = true;
};

#endif // CODEEDITOR_H
