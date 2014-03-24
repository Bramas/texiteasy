#ifndef TEXTDOCUMENTLAYOUT_H
#define TEXTDOCUMENTLAYOUT_H

#include <QPlainTextDocumentLayout>

class WidgetTextEdit;
class TextDocumentLayoutPrivate;

class TextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit TextDocumentLayout(WidgetTextEdit * widgetTextEdit);
    void setTextWidth(qreal newWidth);

signals:

public slots:

protected:
    QSizeF documentSize() const;
    QRectF blockBoundingRect(const QTextBlock &block) const;
    void documentChanged(int from, int, int charsAdded);
private:
    void layoutBlock(const QTextBlock &block);
    qreal blockWidth(const QTextBlock &block);
    qreal indentWidth(const QTextBlock &block);

    TextDocumentLayoutPrivate * d;
    WidgetTextEdit * _widgetTextEdit;
};

#endif // TEXTDOCUMENTLAYOUT_H
