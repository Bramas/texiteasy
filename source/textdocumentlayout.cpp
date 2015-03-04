#include "textdocumentlayout.h"
#include "widgettextedit.h"

#include <QTextBlock>
#include <QString>
#include <QRegExp>
#include <QDebug>


class TextDocumentLayoutPrivate
{
public:
    TextDocumentLayoutPrivate() {
        width = 300;
        maximumWidth = 300;
        maximumWidthBlockNumber = 0;
        blockCount = 1;
        blockUpdate = blockDocumentSizeChanged = false;
    }
    int blockCount;
    int maximumWidth;
    int maximumWidthBlockNumber;
    int width;
    bool blockUpdate, blockDocumentSizeChanged;
};

TextDocumentLayout::TextDocumentLayout(WidgetTextEdit * widgetTextEdit) :
    QPlainTextDocumentLayout(widgetTextEdit->document()),
    _widgetTextEdit(widgetTextEdit)
{
    d = new TextDocumentLayoutPrivate();
}


QRectF TextDocumentLayout::blockBoundingRect(const QTextBlock &block) const
{
    if (!block.isValid()) { return QRectF(); }
    QTextLayout *tl = block.layout();

    if (!tl->lineCount())
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



/*! \reimp
 */
void TextDocumentLayout::documentChanged(int from, int /*charsRemoved*/, int charsAdded)
{
    QTextDocument *doc = document();
    int newBlockCount = doc->blockCount();

    QTextBlock changeStartBlock = doc->findBlock(from);
    QTextBlock changeEndBlock = doc->findBlock(qMax(0, from + charsAdded - 1));

    if (changeStartBlock == changeEndBlock && newBlockCount == d->blockCount) {
        QTextBlock block = changeStartBlock;
        int blockLineCount = block.layout()->lineCount();
        if (block.isValid() && blockLineCount) {
            QRectF oldBr = blockBoundingRect(block);
            layoutBlock(block);
            QRectF newBr = blockBoundingRect(block);
            if (newBr.height() == oldBr.height()) {
                //if (!d->blockUpdate)
                //    emit updateBlock(block);
                return;
            }
        }
    } else {
        QTextBlock block = changeStartBlock;
        do {
            block.clearLayout();
            if (block == changeEndBlock)
                break;
            block = block.next();
        } while(block.isValid());
    }

    if (newBlockCount != d->blockCount) {

        int changeEnd = changeEndBlock.blockNumber();
        int blockDiff = newBlockCount - d->blockCount;
        int oldChangeEnd = changeEnd - blockDiff;

        if (d->maximumWidthBlockNumber > oldChangeEnd)
            d->maximumWidthBlockNumber += blockDiff;

        d->blockCount = newBlockCount;
        if (d->blockCount == 1)
            d->maximumWidth = blockWidth(doc->firstBlock());

        if (!d->blockDocumentSizeChanged)
            emit documentSizeChanged(documentSize());

        if (blockDiff == 1 && changeEnd == newBlockCount -1 ) {
           // if (!d->blockUpdate)
            {
                QTextBlock b = changeStartBlock;
                for(;;) {
                    emit updateBlock(b);
                    if (b == changeEndBlock)
                        break;
                    b = b.next();
                }
            }
            return;
        }
    }
    //if (!d->blockUpdate)
        emit update(QRectF(0., -doc->documentMargin(), 1000000000., 1000000000.)); // optimization potential
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
    qreal availableWidth = d->width;
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
    if (blockMaximumWidth > d->maximumWidth) {
        // new longest line
        d->maximumWidth = blockMaximumWidth;
        d->maximumWidthBlockNumber = block.blockNumber();
        emitDocumentSizeChanged = true;
    } else if (block.blockNumber() == d->maximumWidthBlockNumber && blockMaximumWidth < d->maximumWidth) {
        // longest line shrinking
        QTextBlock b = doc->firstBlock();
        d->maximumWidth = 0;
        QTextBlock maximumBlock;
        while (b.isValid()) {
            qreal blockMaximumWidth = blockWidth(b);
            if (blockMaximumWidth > d->maximumWidth) {
                d->maximumWidth = blockMaximumWidth;
                maximumBlock = b;
            }
            b = b.next();
        }
        if (maximumBlock.isValid()) {
            d->maximumWidthBlockNumber = maximumBlock.blockNumber();
            emitDocumentSizeChanged = true;
        }
    }
    if (emitDocumentSizeChanged)// && !d->blockDocumentSizeChanged)
        emit documentSizeChanged(documentSize());

    emit updateBlock(block);
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

/*
QRectF TextDocumentLayout::blockBoundingRect(const QTextBlock &block) const {
    int blockNumber = block.blockNumber();
    QTextBlock currentBlock = block.document()->firstBlock();
    int currentBlockNumber = currentBlock.blockNumber();
    if (!block.isValid() || !currentBlock.isValid())
        return QRectF();

    QTextDocument *doc = document();

    QPointF offset;
    QRectF r = documentLayout->blockBoundingRect(currentBlock);
    int maxVerticalOffset = r.height();

    while (currentBlockNumber < blockNumber && offset.y() - maxVerticalOffset <= 2* textEdit->viewport()->height()) {
        offset.ry() += r.height();
        currentBlock = currentBlock.next();
        ++currentBlockNumber;
        if (!currentBlock.isVisible()) {
            currentBlock = doc->findBlockByLineNumber(currentBlock.firstLineNumber());
            currentBlockNumber = currentBlock.blockNumber();
        }
        r = documentLayout->blockBoundingRect(currentBlock);
    }
    while (currentBlockNumber > blockNumber && offset.y() + maxVerticalOffset >= -textEdit->viewport()->height()) {
        currentBlock = currentBlock.previous();
        --currentBlockNumber;
        while (!currentBlock.isVisible()) {
            currentBlock = currentBlock.previous();
            --currentBlockNumber;
        }
        if (!currentBlock.isValid())
            break;

        r = documentLayout->blockBoundingRect(currentBlock);
        offset.ry() -= r.height();
    }

    if (currentBlockNumber != blockNumber) {
        // fallback for blocks out of reach. Give it some geometry at
        // least, and ensure the layout is up to date.
        r = documentLayout->blockBoundingRect(block);
        if (currentBlockNumber > blockNumber)
            offset.ry() -= r.height();
    }
    r.translate(offset);
    return r;
}
*/

void TextDocumentLayout::setTextWidth(qreal newWidth)
{
    d->width = d->maximumWidth = newWidth;
    //d->relayout();
}

/*!
  \reimp
 */
QSizeF TextDocumentLayout::documentSize() const
{
    return QSizeF(d->maximumWidth, document()->lineCount());
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
