#ifndef TEXTDOCUMENTLAYOUT_H
#define TEXTDOCUMENTLAYOUT_H

#include <QPlainTextDocumentLayout>

class WidgetTextEdit;
class TextDocumentLayoutPrivate;
class TextDocument;
class TextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit TextDocumentLayout(TextDocument * textDocument);
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
};

#endif // TEXTDOCUMENTLAYOUT_H
