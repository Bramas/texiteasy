#ifndef TEXTDOCUMENTLAYOUT_H
#define TEXTDOCUMENTLAYOUT_H

#include <QPlainTextDocumentLayout>

class QPlainTextDocumentLayoutPrivate;

class TextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
public:
    explicit TextDocumentLayout(QTextDocument *document);

signals:

public slots:

protected:
    QRectF blockBoundingRect(const QTextBlock &block) const;

private:
    void layoutBlock(const QTextBlock &block);
    qreal blockWidth(const QTextBlock &block);
    qreal indentWidth(const QTextBlock &block);
};

#endif // TEXTDOCUMENTLAYOUT_H
