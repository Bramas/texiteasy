#include "textdocumentlayout.h"


#include <QTextBlock>
#include <QString>
#include <QRegExp>
#include <QDebug>


TextDocumentLayout::TextDocumentLayout(QTextDocument *document) :
    QPlainTextDocumentLayout(document)
{
}


QRectF TextDocumentLayout::blockBoundingRect(const QTextBlock &block) const
{
    if (!block.isValid()) { return QRectF(); }
    QTextLayout *tl = block.layout();

    //if (!tl->lineCount())
        const_cast<TextDocumentLayout*>(this)->layoutBlock(block);


    QRectF br;
    if (block.isVisible()) {
        br = QRectF(QPointF(0, 0), tl->boundingRect().bottomRight());
        if (tl->lineCount() == 1)
            br.setWidth(qMax(br.width(), tl->lineAt(0).naturalTextWidth()));
        qreal margin = document()->documentMargin();
        br.adjust(0, 0, margin, 0);
        if (!block.next().isValid())
            br.adjust(0, 0, 0, margin);
    }
    return br;
}



void TextDocumentLayout::layoutBlock(const QTextBlock &block)
{
    QTextDocument *doc = document();
    qreal margin = doc->documentMargin();
    qreal blockMaximumWidth = 0;

    qreal height = 0;
    QTextLayout *tl = block.layout();
    QTextOption option = doc->defaultTextOption();
    tl->setTextOption(option);

    int extraMargin = 0;
    if (option.flags() & QTextOption::AddSpaceForLineAndParagraphSeparators) {
        QFontMetrics fm(block.charFormat().font());
        extraMargin += fm.width(QChar(0x21B5));
    }
    tl->beginLayout();
    qreal availableWidth = 400; //Private
    if (availableWidth <= 0) {
        availableWidth = qreal(INT_MAX); // similar to text edit with pageSize.width == 0
    }
    availableWidth -= 2*margin + extraMargin;
    qreal indentMargin = 0;
    while (1) {
        QTextLine line = tl->createLine();
        if (!line.isValid())
            break;
        line.setLeadingIncluded(true);
        line.setLineWidth(availableWidth - indentMargin);
        line.setPosition(QPointF(margin + indentMargin, height));
        if(!height) //enter only in the first iteration
        {
            indentMargin = indentWidth(block);
        }
        height += line.height();
        blockMaximumWidth = qMax(blockMaximumWidth, line.naturalTextWidth() + 2*margin);
    }
    tl->endLayout();

    int previousLineCount = doc->lineCount();
    const_cast<QTextBlock&>(block).setLineCount(block.isVisible() ? tl->lineCount() : 0);
    int lineCount = doc->lineCount();

    bool emitDocumentSizeChanged = previousLineCount != lineCount;
    if (blockMaximumWidth > 400) {//Private
        // new longest line
        //d->maximumWidth = blockMaximumWidth;
        //d->maximumWidthBlockNumber = block.blockNumber();
        emitDocumentSizeChanged = true;
    } else if (block.blockNumber() == 10 && blockMaximumWidth < 400) {//Private
        // longest line shrinking
        QTextBlock b = doc->firstBlock();
        //d->maximumWidth = 0;
        QTextBlock maximumBlock;
        while (b.isValid()) {
            qreal blockMaximumWidth = blockWidth(b);
            if (blockMaximumWidth > 400) {//Private
                //d->maximumWidth = blockMaximumWidth;
                maximumBlock = b;
            }
            b = b.next();
        }
        if (maximumBlock.isValid()) {
            //d->maximumWidthBlockNumber = maximumBlock.blockNumber();
            emitDocumentSizeChanged = true;
        }
    }
    if (emitDocumentSizeChanged)// && !d->blockDocumentSizeChanged)
        emit documentSizeChanged(documentSize());
}

qreal TextDocumentLayout::blockWidth(const QTextBlock &block)
{
    QTextLayout *layout = block.layout();
    if (!layout->lineCount())
        return 0; // only for layouted blocks
    qreal blockWidth = 0;
    for (int i = 0; i < layout->lineCount(); ++i) {
        QTextLine line = layout->lineAt(i);
        blockWidth = qMax(line.naturalTextWidth() + 8, blockWidth);
    }
    return blockWidth;
}

qreal TextDocumentLayout::indentWidth(const QTextBlock &block)
{
    QRegExp exp("^[ \\t]*");
    if(exp.indexIn(block.text()) == -1)
    {
        return 0;
    }
    QFontMetrics fm(block.charFormat().font());
    return fm.width(exp.capturedTexts().at(0));
}
