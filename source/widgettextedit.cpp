/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                          *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify        *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,             *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.       *                         *
 *                                                                         *
 ***************************************************************************/

#include "widgettextedit.h"
#include "widgetinsertcommand.h"
#include "configmanager.h"
#include "file.h"
#include "viewer.h"
#include <QScrollBar>
#include <QDebug>
#include <QPainter>
#include "filestructure.h"
#include "blockdata.h"
#include <QListIterator>
#include <QMutableListIterator>
#include <QList>
#include <QFontMetrics>
#include <QFileDialog>
#include <QSettings>
#include <QMimeData>
#include <QPalette>
#include "syntaxhighlighter.h"
#include "completionengine.h"
#include <math.h>
#include <QtCore>

#define max(a,b) ((a) < (b) ? (b) : (a))
#define min(a,b) ((a) > (b) ? (b) : (a))
#define abs(a) ((a) > 0 ? (a) : (-(a)))

WidgetTextEdit::WidgetTextEdit(QWidget * parent) :
    QTextEdit(parent),
    _completionEngine(new CompletionEngine(this)),
    currentFile(new File(this)),
    fileStructure(new FileStructure(this)),
    firstVisibleBlock(0),
    _indentationInited(false),
    _lineCount(0),
    _syntaxHighlighter(0),
    updatingIndentation(false),
    _widgetInsertCommand(new WidgetInsertCommand(this))

{

    connect(this,SIGNAL(textChanged()),this->currentFile,SLOT(setModified()));
    connect(this,SIGNAL(textChanged()),this,SLOT(updateIndentation()));
    connect(this,SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChange()));
    //connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(update()));
    connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),this->viewport(),SLOT(update()));

    //this->setCurrentFont(QFont("Consolas", 17));
    //this->setCurrentFont(QFont("Consolas", 17));



    this->setText(" ");
    this->currentFile->setModified(false);

}

void WidgetTextEdit::scrollTo(int p)
{
    this->verticalScrollBar()->setSliderPosition(p);
}

void WidgetTextEdit::setText(const QString &text)
{
    this->_indentationInited = false;
    QTextEdit::setPlainText(text);
/* TODO : Run initIndentation in a thread */
    //QtConcurrent::run(this,&WidgetTextEdit::initIndentation);
    this->initIndentation();
    this->updateIndentation();
    this->viewport()->update();
}
void WidgetTextEdit::insertText(const QString &text)
{
    QTextEdit::insertPlainText(text);
}
void WidgetTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    QPainter painter(viewport());



    painter.setBrush(ConfigManager::Instance.getTextCharFormats("leftStructure").background());
    painter.setPen(QPen(ConfigManager::Instance.getTextCharFormats("leftStructure").foreground().color()));

    QFont font(ConfigManager::Instance.getTextCharFormats("leftStructure").font().family(),ConfigManager::Instance.getTextCharFormats("leftStructure").font().pointSize());
    font.setBold(ConfigManager::Instance.getTextCharFormats("leftStructure").font().bold());
    QFontMetrics fm(font);
    painter.setFont(font);



    QListIterator<FileStructureInfo*> iterator(*this->fileStructure->info());
    FileStructureInfo * value;
    int top = 0;
    int height = 0;
    //qDebug()<<"--------------------";



    painter.translate(15,0);
    painter.rotate(-90);
    while(iterator.hasNext())
    {

        value = iterator.next();
        if(value->top + value->height < this->verticalScrollBar()->value() ||
           this->verticalScrollBar()->value() + this->height() < value->top   )
        {
            continue;
        }
        top = max(value->top - this->verticalScrollBar()->value(),0);
        height = fm.width(value->name);
        //qDebug()<<value->top<<","<<value->endBlock<<"    "<<(value->height + value->top - this->verticalScrollBar()->value())<<" , "<<(height + 30);
        if(value->height + value->top - this->verticalScrollBar()->value() <  height + 30)
        {
            top = value->top + value->height -30 -height - this->verticalScrollBar()->value();
            //qDebug()<<"pas assez "<<(value->height + value->top - this->verticalScrollBar()->value())<<" > "<<(height + 30);
        }

        painter.setPen(QPen(ConfigManager::Instance.getTextCharFormats("leftStructure").background().color()));
        painter.drawRect(- value->top - value->height -4  + this->verticalScrollBar()->value(),25*(value->level-2)+5,value->height-2,25);

        painter.setPen(QPen(ConfigManager::Instance.getTextCharFormats("leftStructure").foreground().color()));
        painter.drawText(-top-height-20,25*(value->level-1),value->name);
    }




    int lastFirstVisibleBlock = this->firstVisibleBlock;
    this->firstVisibleBlock = -1;
    for(int i=1; i< this->document()->blockCount(); ++i)
    {
        //qDebug()<<"block "<<i<<" top : "<<blocksInfo[i].top<<" height : "<<blocksInfo[i].height<<"  scroll : "<<this->verticalScrollBar()->value();
        if(this->firstVisibleBlock == -1 && this->blockBottom(i) > this->verticalScrollBar()->value())
        {
            this->firstVisibleBlock = i;
        }

    }
    if(lastFirstVisibleBlock != this->firstVisibleBlock)
    {
        emit updateFirstVisibleBlock(this->firstVisibleBlock,this->blockTop(this->firstVisibleBlock));
    }
    else
    {
        emit updatedWithSameFirstVisibleBlock();
    }


}

bool WidgetTextEdit::isCursorVisible()
{
    bool down = this->blockBottom(this->textCursor().block()) > this->verticalScrollBar()->value();
    bool up = this->blockTop(this->textCursor().block()) < this->verticalScrollBar()->value()+this->height();
    return up && down;
}

void WidgetTextEdit::onCursorPositionChange()
{
    QList<QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);
    this->highlightCurrentLine();
    matchAll();
    this->currentFile->getViewer()->setLine(this->textCursor().blockNumber()+1);

    this->_widgetInsertCommand->setVisible(false);

    emit cursorPositionChanged(this->textCursor().blockNumber() + 1, this->textCursor().positionInBlock() + 1);
}

void WidgetTextEdit::resizeEvent(QResizeEvent *event)
{
    this->updateIndentation();
    QTextEdit::resizeEvent(event);
    update();
    //this->updateGeometry();
    //this->update();
    //this->viewport()->update();
}

void WidgetTextEdit::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space && (e->modifiers() & Qt::CTRL))
    {
        //this->matchCommand();
        this->displayWidgetInsertCommand();
        return;
    }
    if(this->_completionEngine->isVisible() && e->key() == Qt::Key_Down)
    {
        this->_completionEngine->setFocus();
        return;
    }
    if(this->_completionEngine->isVisible() && e->key() == Qt::Key_Up)
    {
        this->_completionEngine->setFocus();
        return;
    }
    if(e->key() == Qt::Key_Tab)
    {
        if(-1 == this->textCursor().block().text().left(this->textCursor().positionInBlock()).indexOf(QRegExp("^[ \t]*$")))
        {
            if(this->selectNextArgument())
            {
                return;
            }
        }
    }

    if(this->focusWidget() != this)
    {
        QString insertWord = this->_completionEngine->acceptedWord();
        QRegExp command("\\\\[a-zA-Z\\{\\-_]+$");
        int pos = this->textCursor().positionInBlock();
        QString possibleCommand = this->textCursor().block().text().left(pos);


        if(possibleCommand.indexOf(command) != -1) // the possibleCommand is a command
        {
            QTextCursor cur(this->textCursor());
            cur.setPosition(cur.position() - command.matchedLength(), QTextCursor::KeepAnchor);
            cur.removeSelectedText();
            this->insertPlainText(insertWord);
            this->setFocus();
             // place the cursor at the begin of the command in order to search some argumuments
            cur = this->textCursor();
            cur.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, insertWord.length());
            this->setTextCursor(cur);
            if(!this->selectNextArgument()) // if no argument are found, place the cursor at the end of the command
            {
                cur = this->textCursor();
                cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, insertWord.length());
                this->setTextCursor(cur);
            }
        }
        return;
    }
    if(e->key() == Qt::Key_Dollar)
    {
        /* TODO : use beginEditBlock */
        QTextCursor cur = this->textCursor();
        int start = cur.selectionStart();
        int end = cur.selectionEnd();
        BlockData * bd = dynamic_cast<BlockData *>(this->textCursor().block().userData());
        if(start == end && bd->isAClosingDollar(start - this->textCursor().block().position()))
        {
            cur.insertText(QString::fromUtf8("$"));
            this->setTextCursor(cur);
            return;
        }
        cur.beginEditBlock();
        cur.setPosition(start);
        cur.insertText(QString::fromUtf8("$"));
        cur.setPosition(end+1);
        cur.endEditBlock();
        cur.beginEditBlock();
        cur.insertText(QString::fromUtf8("$"));

        if(end == start)
        {
            cur.movePosition(QTextCursor::Left);
        }
        cur.endEditBlock();
        this->setTextCursor(cur);
        return;
    }
    if(e->key() == Qt::Key_BraceLeft)
    {

        QTextCursor cur = this->textCursor();
        int start = cur.selectionStart();
        int end = cur.selectionEnd();
        cur.setPosition(start);
        cur.insertText(QString::fromUtf8("{"));
        cur.setPosition(end+1);
        cur.insertText(QString::fromUtf8("}"));
        if(end == start)
        {
            cur.movePosition(QTextCursor::Left);
        }
        this->setTextCursor(cur);
        return;
    }
    QTextEdit::keyPressEvent(e);
    /*//qDebug()<<"ok"<<e->key()<<"  "<<Qt::Key_Enter;
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Enter - 1)
    {

    }*/
}

bool WidgetTextEdit::selectNextArgument()
{
    //([\{\[\(,])
    QTextCursor cur = this->document()->find(QRegExp("@[^\\}\\]\\),]*"),this->textCursor().position());
    if(cur.isNull())
    {
        return false;
    }
    this->setTextCursor(cur);
    return true;
}

void WidgetTextEdit::wheelEvent(QWheelEvent * event)
{

    if(event->modifiers() & (Qt::ControlModifier))
    {
        int delta =  event->delta() > 0 ? 1 : -1 ;


        //this->textCursor().movePosition()
        ConfigManager::Instance.changePointSizeBy(delta);
        /*this->setCurrentCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
        QTextBlock tb = this->document()->begin();
        for(int i = 0; i < this->document()->blockCount(); ++i)
        {
            tb.layout()->setFont(ConfigManager::Instance.getTextCharFormats("normal").font());
            tb.clearLayout();
            tb.next();
        }*/
        int pos = this->textCursor().position();
        this->selectAll();
        this->textCursor().setBlockCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));

        QTextCursor cur(this->textCursor());
        cur.setPosition(pos);
        cur.setCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
        this->setTextCursor(cur);

        this->setCurrentCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
        this->setCurrentFont(ConfigManager::Instance.getTextCharFormats("normal").font());
        this->setStyleSheet(QString("QTextEdit { border: 1px solid ")+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("textedit-border").foreground().color())+"; "+QString("color: ")+
                                            ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+"; "+
                                            QString("background-color: ")+ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color())+
                                    "; }");

        if(this->_syntaxHighlighter)
        {
            this->_syntaxHighlighter->rehighlight();
        }

        int lastFirstVisibleBlock = this->firstVisibleBlock;
        this->firstVisibleBlock = -1;
        for(int i=1; i < this->document()->blockCount(); ++i)
        {
            if(this->firstVisibleBlock == -1 && this->blockBottom(i) > this->verticalScrollBar()->value())
            {
                this->firstVisibleBlock = i;
            }

        }
        if(lastFirstVisibleBlock != this->firstVisibleBlock)
        {
            emit updateFirstVisibleBlock(this->firstVisibleBlock,this->blockTop(this->firstVisibleBlock));
        }
        else
        {
            emit updateFirstVisibleBlock(this->firstVisibleBlock,this->blockTop(this->firstVisibleBlock));
            //emit updatedWithSameFirstVisibleBlock();
        }
    }
    else
    {
        QTextEdit::wheelEvent(event);
    }
    update();
}
void WidgetTextEdit::setBlockLeftMargin(const QTextBlock &textBlock, int leftMargin)
{
    QTextBlockFormat format;
    QTextCursor cursor(this->textCursor());
    format.setLeftMargin(leftMargin);
    cursor.setPosition(textBlock.position());
    cursor.setBlockFormat(format);
}

void WidgetTextEdit::initIndentation(void)
{
    if(this->updatingIndentation)
    {
        return;
    }
    this->updatingIndentation = true;
    this->fileStructure->updateStructure();
    if(!this->fileStructure->info()->count())
    {

        this->updatingIndentation = false;
        return;
    }

    QTextBlock textBlock = this->document()->begin();


    FileStructureInfo * value;

    QListIterator<FileStructureInfo*> iterator(*this->fileStructure->info());

    BlockInfo * blocksInfo = new BlockInfo[this->document()->blockCount()];
    while(iterator.hasNext())
    {
        value = iterator.next();
        value->top = this->blockTop(value->startBlock);
        value->height = this->blockBottom(value->endBlock) - this->blockTop(value->startBlock);

        for(int i = value->startBlock; i <= value->endBlock; ++i)
        {
            blocksInfo[i].leftMargin = 25*value->level;
        }
    }

    for(int idx = 0; idx < this->document()->blockCount(); ++idx)
    {
        textBlock = this->document()->findBlockByNumber(idx);
        this->setBlockLeftMargin(textBlock, blocksInfo[idx].leftMargin);
    }

    this->currentFile->refreshLineNumber();
    this->_indentationMutex.lock();
    this->_indentationInited = true;
    this->_indentationMutex.unlock();
    this->updatingIndentation = false;
}

void WidgetTextEdit::updateIndentation(void)
{
    if(this->updatingIndentation)
    {
        return;
    }
    this->updatingIndentation = true;


    if(this->document()->blockCount() != _lineCount)
    {
        this->currentFile->insertLine(this->textCursor().blockNumber(), this->document()->blockCount() - _lineCount);
        emit lineCountChanged(this->document()->blockCount());
    }
    _lineCount = this->document()->blockCount();



    this->matchCommand();

    this->_indentationMutex.lock();
    if(!this->_indentationInited)
    {
        this->_indentationMutex.unlock();
        this->updatingIndentation = false;
        return;
    }
    this->_indentationMutex.unlock();

    this->fileStructure->updateStructure();
    if(!this->fileStructure->info()->count())
    {
        this->updatingIndentation = false;
        return;
    }

    QTextBlockFormat myClassFormat;
    QListIterator<FileStructureInfo*> iterator(*this->fileStructure->info());


    FileStructureInfo * value;
    while(iterator.hasNext())
    {
        value = iterator.next();
        value->top = this->blockTop(value->startBlock);
        value->height = this->blockBottom(value->endBlock) - this->blockTop(value->startBlock);

    }

    BlockIndentation * indentation = this->fileStructure->indentations();
    QTextBlock block = this->textCursor().block();
    if(block.blockFormat().leftMargin() != 25*indentation[block.blockNumber()].level)
        for(int i = block.blockNumber(); i < indentation[block.blockNumber()].next; ++i)
        {
            this->setBlockLeftMargin(block, 25*indentation[i].level);
            block = block.next();
        }
    this->updatingIndentation = false;


}

void WidgetTextEdit::insertFromMimeData(const QMimeData *source)
{
    //QMimeData * source2 = new QMimeData();
    //source2->setData(QString("text/plain"),QByteArray(source->text().toLatin1()));
    this->insertPlainText(source->text());
}
void WidgetTextEdit::matchAll()
{
    this->_completionEngine->setVisible(false);
    this->matchPar();
    this->matchLat();
}

void WidgetTextEdit::displayWidgetInsertCommand()
{
    QTextLine line = this->textCursor().block().layout()->lineForTextPosition(this->textCursor().positionInBlock());
    qreal top = line.position().y() + line.height() + 5;
    QRect geo = _widgetInsertCommand->geometry();
    geo.moveTo(QPoint(0, top + this->blockTop(this->textCursor().block())-this->verticalScrollBar()->value()));
    if(geo.bottom() > this->height())
    {
        geo.translate(QPoint(0,-geo.height()-line.height()));
    }
    _widgetInsertCommand->setGeometry(geo);
    _widgetInsertCommand->setVisible(true);
    this->_widgetInsertCommand->show();

}

void WidgetTextEdit::matchCommand()
{
    QRegExp command("\\\\[a-zA-Z\\{\\-_]+$");
    QRegExp beginCommand("\\\\begin\\{([^\\}]+)\\}$");
    int pos = this->textCursor().positionInBlock();
    QString possibleCommand = this->textCursor().block().text().left(pos);


    if(possibleCommand.indexOf(command) != -1) // the possibleCommand is a command
    {
        //QFontMetrics fm(ConfigManager::Instance.getTextCharFormats("normal").font());
        int length = command.matchedLength();
        possibleCommand = possibleCommand.right(length);

        QTextLine line = this->textCursor().block().layout()->lineForTextPosition(this->textCursor().positionInBlock());
        qreal left = line.cursorToX(this->textCursor().positionInBlock());
        qreal top = line.position().y() + line.height() + 5;
        this->_completionEngine->proposeCommand(left,top + this->blockTop(this->textCursor().block())-this->verticalScrollBar()->value(), line.height(),possibleCommand);
        if(this->_completionEngine->isVisible())// && e->key() == Qt::Key_Down)
        {
            this->_completionEngine->setFocus();
        }
    }
    else if(possibleCommand.indexOf(beginCommand) != -1)
    {
        QString environment = beginCommand.capturedTexts().last();
        QString endCommand(QString("\\end{")+environment+"}");
        QTextCursor cur = this->textCursor();
        int start = cur.selectionStart();
        cur.clearSelection();
        QTextBlock nextBlock = this->textCursor().block().next().next();
        if(nextBlock.isValid() && nextBlock.text().contains(endCommand))
        {
            return;
        }
        cur.insertText("\n    ");
        cur.beginEditBlock();
        cur.insertText(QString("\n")+endCommand);
        cur.setPosition(start+5);
        cur.endEditBlock();
        this->setTextCursor(cur);
    }

}

void WidgetTextEdit::matchPar()
{

    QTextBlock textBlock = textCursor().block();
    BlockData *data = static_cast<BlockData *>( textBlock.userData() );
    if ( data ) {
        QVector<ParenthesisInfo *> infos = data->parentheses();
        int pos = textCursor().block().position();

        for (int i=0; i < infos.size(); ++i) {
            ParenthesisInfo *info = infos.at(i);
            int curPos = textCursor().position() - textBlock.position();
            // Clicked on a left parenthesis?
            if ( info->position == curPos-1 && info->character == '{' ) {
                if ( matchLeftPar( textBlock, i+1, 0 ) )
                    createParSelection( pos + info->position );
            }

            // Clicked on a right parenthesis?
            if ( info->position == curPos-1 && info->character == '}' ) {
                if ( matchRightPar( textBlock, i-1, 0 ) )
                    createParSelection( pos + info->position );
            }
        }
    }
}
bool WidgetTextEdit::matchLeftPar(	QTextBlock currentBlock, int index, int numLeftPar )
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<ParenthesisInfo *> infos = data->parentheses();
    int docPos = currentBlock.position();

    // Match in same line?
    for ( ; index<infos.size(); ++index ) {
        ParenthesisInfo *info = infos.at(index);

        if ( info->character == '{' ) {
            ++numLeftPar;
            continue;
        }

        if ( info->character == '}' && numLeftPar == 0 ) {
            createParSelection( docPos + info->position );
            return true;
        } else
            --numLeftPar;
    }

    // No match yet? Then try next block
    currentBlock = currentBlock.next();
    if ( currentBlock.isValid() )
        return matchLeftPar( currentBlock, 0, numLeftPar );

    // No match at all
    return false;
}

bool WidgetTextEdit::matchRightPar(QTextBlock currentBlock, int index, int numRightPar)
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<ParenthesisInfo *> infos = data->parentheses();
    int docPos = currentBlock.position();

    // Match in same line?
    for (int j=index; j>=0; --j ) {
        ParenthesisInfo *info = infos.at(j);

        if ( info->character == '}' ) {
            ++numRightPar;
            continue;
        }

        if ( info->character == '{' && numRightPar == 0 ) {
            createParSelection( docPos + info->position );
            return true;
        } else
            --numRightPar;
    }

    // No match yet? Then try previous block
    currentBlock = currentBlock.previous();
    if ( currentBlock.isValid() ) {

        // Recalculate correct index first
        BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
        QVector<ParenthesisInfo *> infos = data->parentheses();

        return matchRightPar( currentBlock, infos.size()-1, numRightPar );
    }

    // No match at all
    return false;
}

void WidgetTextEdit::createParSelection( int pos )
{
    QList<QTextEdit::ExtraSelection> selections = extraSelections();
    QTextEdit::ExtraSelection selection;
    QTextCharFormat format = selection.format;
    format.setBackground( QColor("#FFFF99") );
    format.setForeground( QColor("#FF0000") );
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition( pos );
    cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
    selection.cursor = cursor;
    selections.append( selection );
    setExtraSelections( selections );
}
void WidgetTextEdit::matchLat()
{
    QTextBlock textBlock = textCursor().block();
    //if (foldableLines.keys().contains(textBlock.blockNumber())) createLatSelection(textBlock.blockNumber(),foldableLines[textBlock.blockNumber()]);
    //else
    {
        BlockData *data = static_cast<BlockData *>( textBlock.userData() );
        if( data )
        {
            QVector<LatexBlockInfo *> infos = data->latexblocks();
            if (infos.size()==0)
            {
                emit setBlockRange(-1,-1);
            }
            for ( int i=0; i<infos.size(); ++i )
            {
                LatexBlockInfo *info = infos.at(i);
                int curPos = textCursor().position() - textBlock.position();
                if ( info->position <= curPos && info->character == 'b' ) matchLeftLat( textBlock, i+1, 0, textBlock.blockNumber());
                if ( info->position <= curPos && info->character == 'e' ) matchRightLat( textBlock, i-1, 0,textBlock.blockNumber());
            }
        }
    }

}

bool WidgetTextEdit::matchLeftLat(	QTextBlock currentBlock, int index, int numLeftLat, int bpos )
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<LatexBlockInfo *> infos = data->latexblocks();

    // Match in same line?
    for ( ; index<infos.size(); ++index ) {
        LatexBlockInfo *info = infos.at(index);

        if ( info->character == 'b' ) {
            ++numLeftLat;
            continue;
        }

        if ( info->character == 'e' && numLeftLat == 0 ) {
            createLatSelection( bpos,currentBlock.blockNumber() );
            return true;
        } else
            --numLeftLat;
    }

    // No match yet? Then try next block
    currentBlock = currentBlock.next();
    if ( currentBlock.isValid() )
        return matchLeftLat( currentBlock, 0, numLeftLat, bpos );

    // No match at all
    return false;
}

bool WidgetTextEdit::matchRightLat(QTextBlock currentBlock, int index, int numRightLat, int epos)
{

    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<LatexBlockInfo *> infos = data->latexblocks();

    // Match in same line?
    for (int j=index; j>=0; --j ) {
        LatexBlockInfo *info = infos.at(j);

        if ( info->character == 'e' ) {
            ++numRightLat;
            continue;
        }

        if ( info->character == 'b' && numRightLat == 0 ) {
            createLatSelection( epos, currentBlock.blockNumber() );
            return true;
        } else
            --numRightLat;
    }

    // No match yet? Then try previous block
    currentBlock = currentBlock.previous();
    if ( currentBlock.isValid() ) {

        // Recalculate correct index first
        BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
        QVector<LatexBlockInfo *> infos = data->latexblocks();

        return matchRightLat( currentBlock, infos.size()-1, numRightLat, epos );
    }

    // No match at all
    return false;
}

void WidgetTextEdit::createLatSelection( int start, int end )
{
    int s=qMin(start,end);
    int e=qMax(start,end);
    emit setBlockRange(s,e);
    //endBlock=e;
}

void WidgetTextEdit::goToLine(int line, QString stringSelected)
{
    QTextCursor cursor(this->textCursor());
    cursor.setPosition(this->document()->findBlockByNumber(line - 1).position());
    this->setTextCursor(cursor);
    if(!stringSelected.isEmpty())
    {
        int index;
        qDebug()<<"search : "<<stringSelected;
        if((index = this->document()->findBlockByNumber(line - 1).text().indexOf(stringSelected)) != -1)
        {
            qDebug()<<"found : "<<index;
            QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();
            QTextEdit::ExtraSelection selection;
            selection.format.setBackground(QColor(255,0,0));
            cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,index);
            cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,stringSelected.length());
            selection.cursor = QTextCursor(cursor);
            selection.cursor.clearSelection();
            extraSelections.append(selection);
            this->setExtraSelections(extraSelections);
        }
    }
}

void WidgetTextEdit::highlightCurrentLine(void)
{
    QList<QTextEdit::ExtraSelection> extraSelections = this->extraSelections();


    QTextCursor cursor = textCursor();
    int blockNumber = cursor.blockNumber();
    cursor.movePosition(QTextCursor::StartOfBlock);
    while(cursor.blockNumber() == blockNumber)
    {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(ConfigManager::Instance.getTextCharFormats("selected-line").background());
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = QTextCursor(cursor);
        selection.cursor.clearSelection();
        extraSelections.append(selection);
        if(!cursor.movePosition(QTextCursor::Down))
        {
            break;
        }
    }

    setExtraSelections(extraSelections);
}
void WidgetTextEdit::highlightSyncedLine(int line)
{
    QList<QTextEdit::ExtraSelection> extraSelections;// = this->extraSelections();


    QTextCursor cursor = textCursor();
    cursor.setPosition(this->document()->findBlockByNumber(line).position());
    while(cursor.blockNumber() == line)
    {
        QTextEdit::ExtraSelection selection;
        selection.format.setBackground(ConfigManager::Instance.getTextCharFormats("synced-line").background());
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = QTextCursor(cursor);
        selection.cursor.clearSelection();
        extraSelections.append(selection);
        if(!cursor.movePosition(QTextCursor::Down))
        {
            break;
        }
    }

    setExtraSelections(extraSelections);
}

int WidgetTextEdit::centerBlockNumber()
{
    int centerBlockNumber = this->firstVisibleBlock;
    while(centerBlockNumber < this->document()->blockCount())
    {
        if(this->blockTop(centerBlockNumber) - this->verticalScrollBar()->value() > this->height() / 2)
        {
            break;
        }
        ++centerBlockNumber;
    }
    return centerBlockNumber - 1;

}
