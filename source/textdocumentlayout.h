#ifndef TEXTDOCUMENTLAYOUT_H
#define TEXTDOCUMENTLAYOUT_H

#include <QPlainTextDocumentLayout>

class TextDocumentLayoutPrivate;

class TextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit TextDocumentLayout(QTextDocument *document);
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
