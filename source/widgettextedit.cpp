/***************************************************************************
 *   copyright       : (C) 2013 by Quentin BRAMAS                          *
 *   http://texiteasy.com                                                  *
 *                                                                         *
 *   This file is part of texiteasy.                                       *
 *                                                                         *
 *   texiteasy is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   texiteasy is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                         *
 ***************************************************************************/


#include "hunspell/hunspell.hxx"
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
#include <QApplication>
#include <QMenu>
#include <QImage>
#include <QLayout>
#include <QMutexLocker>
#include "QTextEdit"
#include "filemanager.h"
#include "widgetlinenumber.h"
#include "widgetfile.h"
#include "textdocumentlayout.h"

#define max(a,b) ((a) < (b) ? (b) : (a))
#define min(a,b) ((a) > (b) ? (b) : (a))
#define abs(a) ((a) > 0 ? (a) : (-(a)))

WidgetTextEdit::WidgetTextEdit(WidgetFile * parent) :
    WIDGET_TEXT_EDIT_PARENT_CLASS(parent),
    _completionEngine(new CompletionEngine(this)),
    currentFile(new File(parent, this)),
    _textStruct(new TextStruct(this)),
    fileStructure(new FileStructure(this)),
    _indentationInited(false),
    _lineCount(0),
    _syntaxHighlighter(0),
    updatingIndentation(false),
    _widgetLineNumber(0),
    _macrosMenu(0),
    _scriptIsRunning(false),
    _lastBlockCount(0)

{
    TextDocumentLayout * layout = new TextDocumentLayout(this);
    this->document()->setDocumentLayout(layout);
    connect(layout, SIGNAL(documentSizeChanged(QSizeF)), this, SLOT(adjustScrollbar(QSizeF)));
    _widgetFile = parent;
    this->setContentsMargins(0,0,0,0);
    connect(this,SIGNAL(textChanged()),this->currentFile,SLOT(setModified()));
    connect(this,SIGNAL(textChanged()),this,SLOT(updateIndentation()));
    connect(this,SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChange()));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
    //connect(this->verticalScrollBar(),SIGNAL(valueChanged(int)),this->viewport(),SLOT(update()));
#ifdef OS_MAC
    _wierdCircumflexCursor = false;
#endif
    this->setText(" ");
    this->currentFile->setModified(false);
    this->updateTabWidth();
    connect(&ConfigManager::Instance, SIGNAL(tabWidthChanged()), this, SLOT(updateTabWidth()));
    updateLineWrapMode();
}
WidgetTextEdit::~WidgetTextEdit()
{
    delete fileStructure;
    delete currentFile;
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete WidgetTextEdit";
#endif
}
void WidgetTextEdit::updateLineWrapMode()
{
    setLineWrapMode(ConfigManager::Instance.isLineWrapped() ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
}

void WidgetTextEdit::adjustScrollbar(QSizeF documentSize)
{
    QScrollBar * hbar = this->horizontalScrollBar();
    hbar->setRange(0, (int)documentSize.width() - viewport()->width());
    hbar->setPageStep(viewport()->width());
}

void WidgetTextEdit::scrollTo(int p)
{
    this->verticalScrollBar()->setSliderPosition(p);
}

void WidgetTextEdit::setText(const QString &text)
{
    this->_indentationInited = false;
    WIDGET_TEXT_EDIT_PARENT_CLASS::setPlainText(text);
/* TODO : Run initIndentation in a thread */
    //QtConcurrent::run(this,&WidgetTextEdit::initIndentation);
    this->initIndentation();
    this->updateIndentation();
    this->viewport()->update();
}
void WidgetTextEdit::insertText(const QString &text)
{
    WIDGET_TEXT_EDIT_PARENT_CLASS::insertPlainText(text);
}

typedef QPair<QString,QPair<int,int> > Argument;

void WidgetTextEdit::paintEvent(QPaintEvent *event)
{

    WIDGET_TEXT_EDIT_PARENT_CLASS::paintEvent(event);
    QPainter painter(viewport());
    painter.setFont(ConfigManager::Instance.getTextCharFormats("normal").font());

    painter.setPen(ConfigManager::Instance.getTextCharFormats("normal").foreground().color());
    if(_multipleEdit.count())
    {
        QTextLine line = _multipleEdit.first().block().layout()->lineForTextPosition(_multipleEdit.first().positionInBlock());
        if(line.isValid())
        {
            qreal left = line.cursorToX(_multipleEdit.first().positionInBlock());;
            qreal top = line.position().y() + line.height() + this->blockTop(_multipleEdit.first().block()) + this->contentOffsetTop();
            QPoint curPoint(left,top);
            QPoint diff(0,line.height());
            painter.drawLine(curPoint - diff, curPoint);
        }
    }
/*
    QBrush defaultBrush(ConfigManager::Instance.getTextCharFormats("normal").background().color().lighter(300));
    QBrush selectedBrush(ConfigManager::Instance.getTextCharFormats("normal").background().color().lighter());
    QPen borderSelectedPen = ConfigManager::Instance.getTextCharFormats("normal").foreground().color().darker();
    QPen borderPen = ConfigManager::Instance.getTextCharFormats("normal").foreground().color();*/
    QBrush defaultBrush(ConfigManager::Instance.getTextCharFormats("argument").background().color());
    QBrush selectedBrush(ConfigManager::Instance.getTextCharFormats("argument:selected").background().color());


    QPen textSelectedPen = ConfigManager::Instance.getTextCharFormats("argument:selected").foreground().color();
    QPen textPen = ConfigManager::Instance.getTextCharFormats("argument").foreground().color();

    QPen borderSelectedPen = ConfigManager::Instance.getTextCharFormats("argument-border:selected").foreground().color();
    QPen borderPen = ConfigManager::Instance.getTextCharFormats("argument-border").foreground().color();

    QTextBlock block = this->document()->firstBlock();

    while(block.isValid())
    {
        BlockData *data = static_cast<BlockData *>(block.userData());
        if(data && data->arguments.count())
        {
            foreach(const Argument &arg, data->arguments)
            {
                QTextLayout * layout = block.layout();
                if(layout)
                {
                    QTextLine line = layout->lineForTextPosition(arg.second.first);
                    QTextLine line2 = layout->lineForTextPosition(arg.second.second);
                    if(line.isValid() && line2.isValid())
                    {
                        bool selected = false;
                        bool extra = false;
                        //foreach(QTextEdit::ExtraSelection sel, extraSelections())
                        foreach(QTextCursor cur, _multipleEdit)
                        {
                            if((cur.position() >= block.position() + arg.second.first
                            && cur.position() <= block.position() + arg.second.second)
                                    || (    cur.hasSelection()
                                        &&  cur.selectionStart() <= block.position() + arg.second.second
                                        &&  cur.selectionEnd() >= block.position() + arg.second.first
                                        ))
                            {
                                painter.setBrush(selectedBrush);
                                painter.setPen(borderSelectedPen);
                                extra = true;
                                selected = true;
                            }
                        }
                        if(!extra)
                        {
                            if((textCursor().position() >= block.position() + arg.second.first
                                && textCursor().position() <= block.position() + arg.second.second)
                                || (    textCursor().hasSelection()
                                    &&  textCursor().selectionStart() <= block.position() + arg.second.second
                                    &&  textCursor().selectionEnd() >= block.position() + arg.second.first
                                    )
                                )
                            {
                                painter.setBrush(selectedBrush);
                                painter.setPen(borderSelectedPen);
                                selected = true;
                            }
                            else
                            {
                                painter.setBrush(defaultBrush);
                                painter.setPen(borderPen);
                            }
                        }

                        qreal xLeft = line.cursorToX(arg.second.first);
                        qreal width = line2.cursorToX(arg.second.second) - xLeft;
                        qreal top = line.position().y() + blockTop(block) + contentOffset().y();
                        qreal height = line2.rect().bottom() - line.position().y();
                        QRectF rF(xLeft, top, width, height);
                        QRect r(xLeft, top, width, height);

                        qreal invisibleMargin = painter.fontMetrics().width("{") / 4.0;

                        painter.drawRoundedRect(rF,5,5);
                        painter.setPen(selected ? textSelectedPen : textPen);

                        painter.drawText(rF.bottomLeft() + QPoint(1 + invisibleMargin, -5), arg.first);
                        //update the rect if it is upside for the arguments that are upside the current text cursor, or below for the arguments that are below the current text cursor.
                        if(block.position() + arg.second.second + 2 < textCursor().position() && r.top() < event->rect().top())
                        {
                            update(r);
                        }
                        if(block.position() + arg.second.first - 2 > textCursor().position() && r.top() > event->rect().top())
                        {
                            update(r);
                        }
                    }
                }
            }
        }
        block = block.next();
    }

    return;
}
void WidgetTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu * defaultMenu = createStandardContextMenu();
    QMenu * menu = new QMenu(this);

    QTextCursor cursor = cursorForPosition(event->pos());
    BlockData *data = static_cast<BlockData *>(cursor.block().userData() );

    if(widgetFile()->spellChecker())
    {
        QTextCodec *codec = QTextCodec::codecForName(widgetFile()->spellCheckerEncoding().toLatin1());
        int blockPos = cursor.block().position();
        int colstart, colend;
        colend = colstart = cursor.positionInBlock();
        if (data && colstart < data->length() && data->characterData[colstart].misspelled==true)
        {
            while (colstart >= 0 && (data->characterData[colstart].misspelled==true))
            {
                --colstart;
            }
            ++colstart;
            while (colend < data->length() && (data->characterData[colend].misspelled==true))
            {
                colend++;
            }
            cursor.setPosition(blockPos+colstart,QTextCursor::MoveAnchor);
            cursor.setPosition(blockPos+colend,QTextCursor::KeepAnchor);
            QString    word          = cursor.selectedText();
            QByteArray encodedString = codec->fromUnicode(word);
            bool check = widgetFile()->spellChecker()->spell(encodedString.data());
            if (!check)
            {
                char ** wlst;
                int ns = widgetFile()->spellChecker()->suggest(&wlst, encodedString.data());
                if (ns > 0)
                {
                    QStringList suggWords;
                    for (int i=0; i < ns; i++)
                    {
                        suggWords.append(codec->toUnicode(wlst[i]));
                    }
                    widgetFile()->spellChecker()->free_list(&wlst, ns);
                    if(!suggWords.contains(word))
                    {
                        this->setTextCursor(cursor);

                        QAction * action;
                        QFont spellmenufont (qApp->font());
                        spellmenufont.setBold(true);
                        foreach (const QString &suggestion, suggWords)
                        {
                            action = new QAction(suggestion, menu);
                            menu->insertAction(menu->actionAt(QPoint(0,0)), action);
                            connect(action, SIGNAL(triggered()), this, SLOT(correctWord()));
                            action->setFont(spellmenufont);
                        }
                        spellmenufont.setBold(false);
                        spellmenufont.setItalic(true);
                        action = new QAction(trUtf8("Ajouter au dictionnaire"), menu);
                        menu->insertAction(menu->actionAt(QPoint(0,0)), action);
                        connect(action, SIGNAL(triggered()), this, SLOT(addToDictionnary()));
                        menu->addSeparator();
                    }

                 }
            }
        }// if the current file is misspelled
    }// if there is a dictionnary

    menu->addActions(defaultMenu->actions());
    menu->addSeparator();
    MacroEngine::Instance.createMacrosMenu(menu);



    menu->exec(event->globalPos());
    delete menu;
}
void WidgetTextEdit::addToDictionnary()
{
    QString newword = textCursor().selectedText();
    ConfigManager::Instance.addToDictionnary(this->widgetFile()->dictionary(), newword);
    QTextCodec *codec = QTextCodec::codecForName(widgetFile()->spellCheckerEncoding().toLatin1());
    this->widgetFile()->spellChecker()->add(codec->fromUnicode(newword).data());
    _syntaxHighlighter->rehighlightBlock(textCursor().block());
}

void WidgetTextEdit::correctWord()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        QString newword = action->text();
        textCursor().removeSelectedText();
        textCursor().insertText(newword);
    }
}

void WidgetTextEdit::updateLineNumber(const QRect &rect, int /*dy*/)
{
    if(!_widgetLineNumber)
    {
        return;
    }
    _widgetLineNumber->setCurrentLine(textCursor().blockNumber());
    _widgetLineNumber->update(0, rect.y(), _widgetLineNumber->width(), rect.height());

}
bool WidgetTextEdit::isCursorVisible()
{
    bool down = this->blockBottom(this->textCursor().block()) + this->contentOffsetTop() > 0;
    bool up = this->blockTop(this->textCursor().block()) +this->contentOffsetTop() < this->height();
    return up && down;
}

void WidgetTextEdit::onCursorPositionChange()
{
    _textStruct->environmentPath(textCursor().position());
    QList<QTextEdit::ExtraSelection> selections;
    setExtraSelections(selections);
    this->highlightCurrentLine();
/*

    QTextCursor cursor(this->textCursor());
    QTextBlockFormat blockFormat = cursor.blockFormat();
    qDebug()<<" ancien "<<blockFormat.leftMargin();
    blockFormat.setLeftMargin(150);
    if(blockFormat.isValid() && !this->textCursor().isNull())
    {
        qDebug()<<"test";
        cursor.setBlockFormat(blockFormat);
        this->setTextCursor(cursor);
    }
*/

    if(!_scriptEngine.cursorsMutex()->tryLock())
    {
        QTimer::singleShot(10, this, SLOT(onCursorPositionChange()));
    }else
    {
        if(!textCursor().hasSelection())
        {
            // if the cursor is positionned on an argument => we select the whole argument
            BlockData *data = static_cast<BlockData *>( textCursor().block().userData() );
            if (data) {
                if((textCursor().positionInBlock() < data->characterData.count() && data->characterData[textCursor().positionInBlock()].state == SyntaxHighlighter::CompletionArgument)
                   || (textCursor().positionInBlock() > 0 && data->characterData[textCursor().positionInBlock()-1].state == SyntaxHighlighter::CompletionArgument))
                {
                    QString text = textCursor().block().text();
                    int selStart = textCursor().positionInBlock() - 1;
                    while(selStart > 0 && data->characterData[selStart].state == SyntaxHighlighter::CompletionArgument)
                    {
                        --selStart;
                    }
                    QTextCursor cursor = textCursor();
                    cursor.setPosition(textCursor().block().position() + selStart);
                    this->setTextCursor(cursor);
                    this->selectNextArgument();
                    _scriptEngine.cursorsMutex()->unlock();
                    return;
                }
            }
        }
        if(textCursor().hasSelection())
        {
            //if part of the selection include a part of an argument => enlarge the selection to include all the argument
            //TODO: clean this part
            QTextCursor cStart = textCursor();
            cStart.setPosition(qMin(textCursor().selectionStart(), textCursor().selectionEnd()));
            QTextCursor cEnd = textCursor();
            cEnd.setPosition(qMax(textCursor().selectionStart(), textCursor().selectionEnd()));
            BlockData *dataStart = static_cast<BlockData *>( cStart.block().userData() );
            BlockData *dataEnd = static_cast<BlockData *>( cEnd.block().userData() );
            bool change = false;
            if (dataStart) {
                while(cStart.positionInBlock() > 0 && cStart.positionInBlock() < dataStart->characterData.size()
                      && dataStart->characterData[cStart.positionInBlock()].state == SyntaxHighlighter::CompletionArgument
                      && dataStart->characterData[cStart.positionInBlock() - 1].state == SyntaxHighlighter::CompletionArgument)
                {
                    change = true;
                    cStart.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
                }
            }
            if (dataEnd) {
                while(cEnd.positionInBlock() > 0 && cEnd.positionInBlock() < cEnd.block().text().length()
                      && dataEnd->characterData[cEnd.positionInBlock() - 1].state == SyntaxHighlighter::CompletionArgument
                      && dataEnd->characterData[cEnd.positionInBlock()].state == SyntaxHighlighter::CompletionArgument)
                {
                    change = true;
                    cEnd.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
                }
            }
            if(change)
            {
                QTextCursor cursor = textCursor();
                if(cursor.selectionStart() < cursor.position())
                {
                    cursor.setPosition(cStart.position());
                    cursor.setPosition(cEnd.position(), QTextCursor::KeepAnchor);
                    this->setTextCursor(cursor);
                }
                else
                {
                    cursor.setPosition(cEnd.position());
                    cursor.setPosition(cStart.position(), QTextCursor::KeepAnchor);
                    this->setTextCursor(cursor);
                }
                _scriptEngine.cursorsMutex()->unlock();
                return;
            }
        }
        _scriptEngine.cursorsMutex()->unlock();
    }
    // else we do the usual match

    matchAll();
    this->currentFile->getViewer()->setLine(this->textCursor().blockNumber()+1);

    WidgetInsertCommand::instance()->setVisible(false);

    emit cursorPositionChanged(this->textCursor().blockNumber() + 1, this->textCursor().positionInBlock() + 1);
}

void WidgetTextEdit::resizeEvent(QResizeEvent *event)
{
    WIDGET_TEXT_EDIT_PARENT_CLASS::resizeEvent(event);
    TextDocumentLayout* layout = dynamic_cast<TextDocumentLayout*>(this->document()->documentLayout());
    layout->setTextWidth(viewport()->width());
}

void WidgetTextEdit::insertPlainText(const QString &text)
{
    if(_multipleEdit.count() && !text.contains(QRegExp("[^a-zA-Z0-9 ]")))
    {
        QTextCursor cur1 = this->textCursor();
        cur1.insertText(text);
        _multipleEdit.first().insertText(text);
        this->setTextCursor(cur1);
        return;
    }
    WIDGET_TEXT_EDIT_PARENT_CLASS::insertPlainText(text);
}

void WidgetTextEdit::keyPressEvent(QKeyEvent *e)
{
#ifdef OS_MAC
    _wierdCircumflexCursor = false;
    // Hack because on mac modifiers do not work with a external mouse wheel
    // So we keep the modifiers here so that other widget can ask modifiers (used in widgetpdfdocument::wheelEvent() )
    if(e->key() == Qt::Key_Control)
    {
        modifiers = e->modifiers() | Qt::ControlModifier;
    }else
    if(e->key() == Qt::Key_Alt)
    {
        modifiers = e->modifiers() | Qt::AltModifier;
    }
#endif
    if(e->key() == Qt::Key_Space && (e->modifiers() & (Qt::MetaModifier | Qt::ControlModifier)))
    {
        //this->matchCommand();
        _multipleEdit.clear();
        this->displayWidgetInsertCommand();
        return;
    }
    if(this->_completionEngine->isVisible() && e->key() == Qt::Key_Down)
    {
        _multipleEdit.clear();
        this->_completionEngine->setFocus();
        return;
    }
    if(this->_completionEngine->isVisible() && e->key() == Qt::Key_Up)
    {
        _multipleEdit.clear();
        this->_completionEngine->setFocus();
        return;
    }
    if(this->focusWidget() != this || (_completionEngine->isVisible() && e->key() == Qt::Key_Tab))
    {
        _multipleEdit.clear();
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
    if(e->key() == Qt::Key_Tab)
    {
        _multipleEdit.clear();
        QTextCursor cursor = this->textCursor();
        if(-1 == cursor.block().text().left(cursor.positionInBlock()).indexOf(QRegExp("^[ \t]*$")))
        {
            if(this->selectNextArgument())
            {
                return;
            }
            if(triggerTabMacros())
            {
                return;
            }
            if(cursor.positionInBlock() < cursor.block().text().size())
            {
                QChar c = cursor.block().text().at(cursor.positionInBlock());
                if(c == '}' || c == ']')
                {
                    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
                    this->setTextCursor(cursor);
                    return;
                }
            }
        }
        if(cursor.hasSelection())
        {
            indentSelectedText();
            return;
        }
        insertPlainText(ConfigManager::Instance.tabToString());
        return;
    }
    if(e->key() == Qt::Key_Dollar)
    {
        QTextCursor cur = this->textCursor();
        int start = cur.selectionStart();
        int end = cur.selectionEnd();
        BlockData * bd = dynamic_cast<BlockData *>(this->textCursor().block().userData());
        if(!bd)
        {
            cur.insertText(QString::fromUtf8("$"));
            this->setTextCursor(cur);
            return;
        }
        if(start == end && ConfigManager::Instance.isDollarAuto() && this->nextChar(cur) == '$' && bd->isAClosingDollar(start - this->textCursor().block().position() - 1))
        {
              cur.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor);
              this->setTextCursor(cur);
              return;
        }
        if(start == end && (!ConfigManager::Instance.isDollarAuto() || bd->isAClosingDollar(start - this->textCursor().block().position()) || (bd->characterData.size() && bd->characterData.last().state == SyntaxHighlighter::Math)))
        {
            cur.insertText(QString::fromUtf8("$"));
            this->setTextCursor(cur);
            return;
        }
        cur.beginEditBlock();
        cur.setPosition(start);
        cur.insertText(QString::fromUtf8("$"));
        cur.setPosition(end+1);
        cur.insertText(QString::fromUtf8("$"));

        if(end == start)
        {
            cur.movePosition(QTextCursor::Left);
        }
        cur.endEditBlock();
        this->setTextCursor(cur);
        _multipleEdit.clear();
        return;
    } else
    if(e->key() == Qt::Key_BraceLeft)
    {

        QTextCursor cur = this->textCursor();
        cur.beginEditBlock();
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
        cur.endEditBlock();
        this->setTextCursor(cur);
        _multipleEdit.clear();
        return;
    }
    else if ((e->key()==Qt::Key_Enter)||(e->key()==Qt::Key_Return))
    {
        _multipleEdit.clear();
        QPlainTextEdit::keyPressEvent(e);
        // add exactly the same  space and tabulation as the previous line.
        newLine();
        return;
    }
    if(_scriptIsRunning && !hasArguments() && e->key() != Qt::Key_Backspace && (e->text().isEmpty() || e->text().contains(QRegExp(QString::fromUtf8("[^a-zA-Z0-9èéàëêïîùüû&()\"'\\$§,;\\.+=\\-_*\\/\\\\!?%#@° ]")))))
    {
        _scriptEngine.clear();
        _scriptIsRunning = false;
    }
    if(_multipleEdit.count() && e->modifiers() == Qt::NoModifier && !e->text().isEmpty() && !e->text().contains(QRegExp(QString::fromUtf8("[^a-zA-Z0-9èéàëêïîùüû&()\"'\\$§,;\\.+=\\-_*\\/\\\\!?%#@° ]"))))
    {
        QTextCursor cur1 = this->textCursor();
        QTextCursor cur2 = _multipleEdit.first();
        cur1.beginEditBlock();
        cur1.insertText(e->text());
        cur1.endEditBlock();
        cur2.joinPreviousEditBlock();
        cur2.insertText(e->text());
        cur2.endEditBlock();
        this->setTextCursor(cur1);
        this->onCursorPositionChange();
        return;
    }
    if(_multipleEdit.count() && e->modifiers() == Qt::NoModifier && (e->key() == Qt::Key_Delete || e->key() == Qt::Key_Backspace))
    {
        QTextCursor cur1 = this->textCursor();
        QTextCursor cur2 = _multipleEdit.first();
        cur1.beginEditBlock();
        if(e->key() == Qt::Key_Delete)
        {
            cur1.deleteChar();
            cur1.endEditBlock();
            cur2.joinPreviousEditBlock();
            cur2.deleteChar();
            cur2.endEditBlock();
        }
        else
        {
            cur1.deletePreviousChar();
            cur1.endEditBlock();
            cur2.joinPreviousEditBlock();
            cur2.deletePreviousChar();
            cur2.endEditBlock();
        }
        this->setTextCursor(cur1);
        this->onCursorPositionChange();

        return;
    }
    if(e->key() != Qt::Key_Control && e->key() != Qt::Key_Shift && e->key() != Qt::Key_Alt && e->key() != Qt::Key_AltGr && e->key() != Qt::Key_ApplicationLeft && e->key() != Qt::Key_ApplicationRight)
    {
        _multipleEdit.clear();
    }

    if (e->key() ==  Qt::Key_Backspace && !textCursor().hasSelection())
    {
        if(ConfigManager::Instance.isUsingSpaceIndentation())
        {
            QTextCursor cursor = textCursor();
            if(cursor.block().text().left(cursor.positionInBlock()).contains(QRegExp("^[ ]+$")))
            {
                deletePreviousTab();
                return;
            }
        }
    }
    WIDGET_TEXT_EDIT_PARENT_CLASS::keyPressEvent(e);
    if(e->modifiers() == Qt::NoModifier && !e->text().isEmpty())
    {
        this->matchCommand();
    }
    if(e->key() == Qt::Key_Right)
    {
        onCursorPositionChange();
    }

}
bool WidgetTextEdit::hasArguments()
{
    QTextCursor curStrArg = this->document()->find(QRegExp("%#\\{\\{\\{[^\\}]*\\}\\}\\}#"));
    return !curStrArg.isNull();
}

bool WidgetTextEdit::selectNextArgument()
{
    //QTextCursor curIntArg = this->document()->find(QRegExp("%[0-9]"),this->textCursor().position());
    QTextCursor curStrArg = this->document()->find(QRegExp("%#\\{\\{\\{[^\\}]*\\}\\}\\}#"),this->textCursor().position());

  /*  if(!curIntArg.isNull() && (curStrArg.isNull() || curIntArg.selectionStart() < curStrArg.selectionStart()))
    {
        this->setTextCursor(curIntArg);
        return true;
    }*/
    if(!curStrArg.isNull())
    {
        QTextCursor curStrArg2 = this->document()->find(curStrArg.selectedText(), curStrArg.position() + 1);
        if(curStrArg2.isNull())
        {
            curStrArg2 = this->document()->find(curStrArg.selectedText(), 0);
            if(curStrArg2.position() == curStrArg.position())
            {
                curStrArg2 = QTextCursor();
            }
        }
        this->setTextCursor(curStrArg);
        if(!curStrArg2.isNull())
        {
            QList<QTextEdit::ExtraSelection> selections = extraSelections();
            QTextEdit::ExtraSelection selection;
            QTextCharFormat format = selection.format;
            format.setBackground( QColor("#DDDDDD") );
            format.setForeground( QColor("#333333") );
            selection.format = format;
            selection.cursor = curStrArg2;
            selections.append( selection );
            setExtraSelections( selections );

            _multipleEdit.clear();
            _multipleEdit.append(curStrArg2);
        }
        return true;
    }
    return false;

}

void WidgetTextEdit::wheelEvent(QWheelEvent * event)
{

    if(event->modifiers() & (Qt::ControlModifier))
    {
        int delta =  event->delta() > 0 ? 1 : -1 ;

        ConfigManager::Instance.changePointSizeBy(delta);

        int pos = this->textCursor().position();
        this->selectAll();
        this->textCursor().setBlockCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));

        QTextCursor cur(this->textCursor());
        cur.setPosition(pos);
        cur.setCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
        this->setTextCursor(cur);

        this->setCurrentCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));


        if(this->_syntaxHighlighter)
        {
            this->_syntaxHighlighter->rehighlight();
        }
        _widgetLineNumber->updateWidth(this->document()->blockCount());

    }
    else
    {
        WIDGET_TEXT_EDIT_PARENT_CLASS::wheelEvent(event);
    }
    //update();
}
void WidgetTextEdit::setBlockLeftMargin(const QTextBlock &textBlock, int leftMargin)
{
    QTextBlockFormat format;
    format = textCursor().blockFormat();
    qDebug()<<"oldMargin "<<format.leftMargin()<<" new "<<leftMargin;
    format.setLeftMargin(leftMargin);
    textCursor().setBlockFormat(format);
}

void WidgetTextEdit::initIndentation(void)
{
    this->currentFile->refreshLineNumber();
}

void WidgetTextEdit::updateIndentation(void)
{
    _textStruct->reload();
    //_textStruct->debug();

    if(_scriptIsRunning)
    {
        _scriptEngine.evaluate();
    }


    if(this->document()->blockCount() != _lineCount)
    {
        this->currentFile->insertLine(this->textCursor().blockNumber(), this->document()->blockCount() - _lineCount);
        emit lineCountChanged(this->document()->blockCount());
    }
    _lineCount = this->document()->blockCount();

}
void WidgetTextEdit::insertFile(QString filename)
{
    // if it is an image, compute the boundingbox option (very usefull for those who use latex + ...
    QImage image(filename);
    QString options = "width=10cm";
    if(!image.isNull())
    {
        options  +=   ", bb=0 0 "+QString::number(image.width())
                    +" "+QString::number(image.height());
    }

    if(!this->getCurrentFile()->getFilename().isEmpty())
    {
        QDir dir(this->getCurrentFile()->getPath());
        filename = dir.relativeFilePath(filename);
    }
    QTextCursor cur = textCursor();
    cur.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
    cur.insertText("\n\\includegraphics["+options+"]{"+filename+"}");
    this->setTextCursor(cur);
}

void WidgetTextEdit::insertFromMimeData(const QMimeData *source)
{
    if(source->hasUrls())
    {
        if(FileManager::Instance.handleMimeData(source))
        {
            return;
        }
    }
    WIDGET_TEXT_EDIT_PARENT_CLASS::insertFromMimeData(source);
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
    if(!line.isValid())
    {
        return;
    }
    qreal top = line.position().y() + line.height() + 5  + this->blockTop(this->textCursor().block()) + this->contentOffsetTop();
    QRect geo = WidgetInsertCommand::instance()->geometry();
    geo.moveTo(QPoint(0, top));
    if(geo.bottom() > this->height())
    {
        geo.translate(QPoint(0,-geo.height()-line.height()));
    }
    WidgetInsertCommand::instance()->setParent(this);
    WidgetInsertCommand::instance()->setGeometry(geo);
    WidgetInsertCommand::instance()->setVisible(true);
    WidgetInsertCommand::instance()->show();

}

void WidgetTextEdit::matchCommand()
{
#ifdef OS_MAC
    if(_wierdCircumflexCursor)
    {
        return;
    }
#endif
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
        if(!line.isValid())
        {
            return;
        }
        qreal left = line.cursorToX(this->textCursor().positionInBlock());
        qreal top = line.position().y() + line.height() + 5;
        this->_completionEngine->proposeCommand(left,top + this->blockTop(this->textCursor().block()) + this->contentOffsetTop(), line.height(),possibleCommand);
        if(this->_completionEngine->isVisible())// && e->key() == Qt::Key_Down)
        {
            this->_completionEngine->setFocus();
        }
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
            if ( info->position <= curPos-1 && info->position + info->length > curPos-1 && !(info->type & ParenthesisInfo::RIGHT) ) {
                if ( matchLeftPar(textBlock, info->type, i+1, 0 ) )
                    createParSelection( pos + info->position, info->length );
            }

            // Clicked on a right parenthesis?
            if ( info->position <= curPos-1 && info->position + info->length > curPos-1 && (info->type & ParenthesisInfo::RIGHT)) {
                if ( matchRightPar( textBlock, info->type, i-1, 0 ) )
                    createParSelection( pos + info->position, info->length );
            }
        }
    }
}
bool WidgetTextEdit::matchLeftPar(	QTextBlock currentBlock, int type, int index, int numLeftPar )
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<ParenthesisInfo *> infos = data->parentheses();
    int docPos = currentBlock.position();

    // Match in same line?
    for ( ; index<infos.size(); ++index ) {
        ParenthesisInfo *info = infos.at(index);

        if ( info->type == type ) {
            ++numLeftPar;
            continue;
        }

        if ( info->type == type + ParenthesisInfo::RIGHT )
        {
            if(numLeftPar == 0) {
                createParSelection( docPos + info->position, info->length );
                return true;
            }
            else
            {
                --numLeftPar;
            }
        }

    }

    // No match yet? Then try next block
    currentBlock = currentBlock.next();
    if ( currentBlock.isValid() )
        return matchLeftPar( currentBlock, type, 0, numLeftPar );

    // No match at all
    return false;
}

bool WidgetTextEdit::matchRightPar(QTextBlock currentBlock, int type, int index, int numRightPar)
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<ParenthesisInfo *> infos = data->parentheses();
    int docPos = currentBlock.position();

    // Match in same line?
    for (int j=index; j>=0; --j ) {
        ParenthesisInfo *info = infos.at(j);

        if ( info->type == type ) {
            ++numRightPar;
            continue;
        }

        if ( info->type == type - ParenthesisInfo::RIGHT)
        {
            if( numRightPar == 0 ) {
                createParSelection( docPos + info->position, info->length );
                return true;
            }
            else
            {
                --numRightPar;
            }
        }
    }

    // No match yet? Then try previous block
    currentBlock = currentBlock.previous();
    if ( currentBlock.isValid() ) {

        // Recalculate correct index first
        BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
        QVector<ParenthesisInfo *> infos = data->parentheses();

        return matchRightPar( currentBlock, type, infos.size()-1, numRightPar );
    }

    // No match at all
    return false;
}

void WidgetTextEdit::createParSelection( int pos, int length )
{
    QList<QTextEdit::ExtraSelection> selections = extraSelections();
    QTextEdit::ExtraSelection selection;
    QTextCharFormat format = selection.format;
    format.setBackground( QColor("#FFFF99") );
    format.setForeground( QColor("#FF0000") );
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition( pos );
    cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor, length);
    selection.cursor = cursor;
    selections.append( selection );
    setExtraSelections( selections );
}
void WidgetTextEdit::matchLat()
{
    QTextBlock textBlock = textCursor().block();
    QString lineBegining = textBlock.text().left(textCursor().selectionEnd() - textBlock.position() + 1);
    int envLength = textCursor().selectionEnd() - textCursor().selectionStart();
    bool cursorIsAtTheStart = textCursor().selectionStart() == textCursor().position();
    int indexEnv;
    QRegExp envBeginPattern("\\\\begin\\{[^\\}]{"+QString::number(envLength)+"}\\}$");
    QRegExp envEndPattern("\\\\end\\{[^\\}]{"+QString::number(envLength)+"}\\}$");

    int inEnv = -1;
    if((indexEnv = lineBegining.indexOf(envBeginPattern)) != -1)
    {
        inEnv = envBeginPattern.matchedLength() - 8;
    }
    if((indexEnv = lineBegining.indexOf(envEndPattern)) != -1)
    {
        inEnv = envEndPattern.matchedLength() - 6;
    }

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
                if ( info->position <= curPos && info->type == LatexBlockInfo::ENVIRONEMENT_BEGIN )
                {
                    int associatedEnv = matchLeftLat( textBlock, i+1, 0, textBlock.blockNumber());
                    if(inEnv != -1 && associatedEnv != -1)
                    {

                                QList<QTextEdit::ExtraSelection> selections = extraSelections();
                                QTextEdit::ExtraSelection selection;
                                QTextCharFormat format = selection.format;
                                format.setBackground( QColor("#DDDDDD") );
                                format.setForeground( QColor("#333333") );
                                selection.format = format;
                                QTextCursor cursor = textCursor();
                                cursor.setPosition( associatedEnv + 5 + (cursorIsAtTheStart ? 0 : inEnv));
                                cursor.movePosition( (cursorIsAtTheStart ? QTextCursor::NextCharacter : QTextCursor::PreviousCharacter), QTextCursor::KeepAnchor, inEnv );
                                selection.cursor = cursor;
                                if(!cursor.selectedText().compare(this->textCursor().selectedText()))
                                {
                                    selections.append( selection );
                                    setExtraSelections( selections );
                                    _multipleEdit.append(cursor);
                                }

                    }
                }
                if ( info->position <= curPos && info->type == LatexBlockInfo::ENVIRONEMENT_END )
                {
                    int associatedEnv =  matchRightLat( textBlock, i-1, 0,textBlock.blockNumber());
                    if(inEnv != -1 && associatedEnv != -1)
                    {
                                QList<QTextEdit::ExtraSelection> selections = extraSelections();
                                QTextEdit::ExtraSelection selection;
                                QTextCharFormat format = selection.format;
                                format.setBackground( QColor("#DDDDDD") );
                                format.setForeground( QColor("#333333") );
                                selection.format = format;
                                QTextCursor cursor = textCursor();
                                cursor.setPosition( associatedEnv + (cursorIsAtTheStart ? 0 : inEnv) + 7);
                                cursor.movePosition( (cursorIsAtTheStart ? QTextCursor::NextCharacter : QTextCursor::PreviousCharacter), QTextCursor::KeepAnchor, inEnv );
                                selection.cursor = cursor;
                                if(!cursor.selectedText().compare(this->textCursor().selectedText()))
                                {
                                    selections.append( selection );
                                    setExtraSelections( selections );
                                    _multipleEdit.append(cursor);
                                }
                    }
                }
            }
        }
    }

}

int WidgetTextEdit::matchLeftLat(	QTextBlock currentBlock, int index, int numLeftLat, int bpos )
{
    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<LatexBlockInfo *> infos = data->latexblocks();

    // Match in same line?
    for ( ; index<infos.size(); ++index ) {
        LatexBlockInfo *info = infos.at(index);

        if ( info->type == LatexBlockInfo::ENVIRONEMENT_BEGIN ) {
            ++numLeftLat;
            continue;
        }

        if ( info->type == LatexBlockInfo::ENVIRONEMENT_END && numLeftLat == 0 ) {
            createLatSelection( bpos,currentBlock.blockNumber() );
            return info->position + currentBlock.position();
        } else
            --numLeftLat;
    }

    // No match yet? Then try next block
    currentBlock = currentBlock.next();
    if ( currentBlock.isValid() )
        return matchLeftLat( currentBlock, 0, numLeftLat, bpos );

    // No match at all
    return -1;
}

int WidgetTextEdit::matchRightLat(QTextBlock currentBlock, int index, int numRightLat, int epos)
{

    BlockData *data = static_cast<BlockData *>( currentBlock.userData() );
    QVector<LatexBlockInfo *> infos = data->latexblocks();

    // Match in same line?
    for (int j=index; j>=0; --j ) {
        LatexBlockInfo *info = infos.at(j);

        if ( info->type == LatexBlockInfo::ENVIRONEMENT_END ) {
            ++numRightLat;
            continue;
        }

        if ( info->type == LatexBlockInfo::ENVIRONEMENT_BEGIN && numRightLat == 0 ) {
            createLatSelection( epos, currentBlock.blockNumber() );
            return info->position + currentBlock.position();
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
    return -1;
}

QChar WidgetTextEdit::nextChar(const QTextCursor cursor) const
{
    QTextBlock block = cursor.block();
    int position = cursor.positionInBlock();
    return block.text().at(position);
}

void WidgetTextEdit::createLatSelection( int start, int end )
{
    int s=qMin(start,end);
    int e=qMax(start,end);
    emit setBlockRange(s,e);
    //endBlock=e;
}

void WidgetTextEdit::updateTabWidth()
{
    QFontMetrics fm(ConfigManager::Instance.getTextCharFormats("normal").font());
    WIDGET_TEXT_EDIT_PARENT_CLASS::setTabStopWidth(ConfigManager::Instance.tabWidth() * fm.width(" "));
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
    int centerBlockNumber = this->firstVisibleBlock().blockNumber();
    while(centerBlockNumber < this->document()->blockCount())
    {
        if(this->contentOffset().y() + this->blockTop(centerBlockNumber) > this->height() / 2)
        {
            break;
        }
        ++centerBlockNumber;
    }
    return centerBlockNumber - 1;

}

QString WidgetTextEdit::wordOnLeft()
{
    QTextCursor cursor = textCursor();
    if(cursor.hasSelection())
    {
        return cursor.selectedText();
    }

    BlockData *data = static_cast<BlockData *>(cursor.block().userData() );
    if(data && cursor.positionInBlock() > 0 && data->characterData[cursor.positionInBlock() - 1].state == SyntaxHighlighter::Command)
    {
        return "";
    }
    QString lineBegining = cursor.block().text().left(cursor.positionInBlock());
    QRegExp lastWordPatter("([a-zA-Z0-9èéàëêïîùüû\\-_*]+)$");
    if(lineBegining.indexOf(lastWordPatter) != -1)
    {
        return lastWordPatter.capturedTexts().last();
    }
    return QString();
}
void WidgetTextEdit::deletePreviousTab()
{
    QTextCursor cursor = textCursor();
    if(cursor.block().text().left(cursor.positionInBlock()).contains(QRegExp("^[ ]*$")))
    {
        cursor.joinPreviousEditBlock();
        cursor.deletePreviousChar(); //delete at least one char
        while(cursor.positionInBlock() % ConfigManager::Instance.tabWidth())
        {
            cursor.deletePreviousChar();
        }
        cursor.endEditBlock();
    }
}

void WidgetTextEdit::newLine()
{
    QTextCursor cursor = textCursor();
    cursor.joinPreviousEditBlock();
    QTextBlock block=cursor.block();
    QTextBlock blockprev=block.previous();
    if(blockprev.isValid())
    {
        QString txt=blockprev.text();
        int j=0;
        while ( (j<txt.count()) && ((txt[j]==' ') || txt[j]=='\t') )
        {
            cursor.insertText(QString(txt[j]));
            j++;
        }

    }
    cursor.endEditBlock();
}

void WidgetTextEdit::setTextCursorPosition(int pos)
{
    QTextCursor cursor = this->textCursor();
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
}

bool WidgetTextEdit::triggerTabMacros()
{
    QList<Macro> list = MacroEngine::Instance.tabMacros();
    foreach(const Macro &macro, list)
    {
        if(onMacroTriggered(macro, true))
        {
            return true;
        }
    }
    return false;
}

bool WidgetTextEdit::onMacroTriggered(Macro macro, bool soft)
{
    QTextCursor cursor = textCursor();
    QString word;
    if(cursor.hasSelection())
    {
        word = cursor.selectedText();
    }
    else
    {
        word = this->wordOnLeft();
    }
    QRegExp pattern("^"+macro.leftWord+"$");
    bool patternExists = word.contains(pattern);
    if(!patternExists && soft)
    {
        return false;
    }
    QString content = macro.content;

    if(patternExists && !cursor.hasSelection())
    {
         cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, word.length());
         this->setTextCursor(cursor);
    }
    int pos = cursor.position();

    _scriptEngine.parse(content, this);
    _scriptIsRunning = true;

    if(patternExists)
    {
        QStringList cap = pattern.capturedTexts();
        cap.pop_front();
        int idx = 1;
        while(cap.count())
        {
            QString arg = cap.front();
            cap.pop_front();
            if(!arg.isEmpty())
            {
                cursor = document()->find(QRegExp("\\$\\{"+QString::number(idx)+":[^\\}]*\\}"));
                while(!cursor.isNull())
                {
                    cursor.removeSelectedText();
                    cursor.insertText(arg);
                    cursor = document()->find(QRegExp("\\$\\{"+QString::number(idx)+":[^\\}]*\\}"));
                }
            }
            ++idx;
        }
    }
    cursor = document()->find(QRegExp("^#[^\\n]*\n"));
    while(!cursor.isNull())
    {
        cursor.removeSelectedText();
        cursor = document()->find(QRegExp("^#[^\\n]*\n"));
    }
    cursor = document()->find(QRegExp("\n#[^\\n]*\n"));
    while(!cursor.isNull())
    {
        cursor.removeSelectedText();
        cursor.insertText("\n");
        cursor = document()->find(QRegExp("\n#[^\\n]*\n"));
    }

    QRegExp argumentPattern("\\$\\{([0-9]:){0,1}([^\\}]*)\\}");
    cursor = document()->find(argumentPattern);
    while(!cursor.isNull())
    {
        cursor.selectedText().indexOf(argumentPattern);
        cursor.removeSelectedText();
        cursor.insertText("%#{{{"+argumentPattern.capturedTexts().at(2)+"}}}#");
        cursor = document()->find(argumentPattern);
    }

    cursor = textCursor();
    cursor.setPosition(pos);
    this->setTextCursor(cursor);
    _multipleEdit.clear();
    selectNextArgument();
    return true;

}

void WidgetTextEdit::indentSelectedText()
{
    QTextCursor cursor = textCursor();
    int startPos = cursor.selectionStart();
    int endPos = cursor.selectionEnd();
    QString tabString = ConfigManager::Instance.tabToString();

    cursor.setPosition(startPos);
    QTextBlock block = cursor.block();
    while(block.isValid() && block.position() <= endPos)
    {
        cursor.setPosition(block.position());
        cursor.insertText(tabString);
        endPos += tabString.size();
        block = block.next();
    }
}
void WidgetTextEdit::initTheme()
{

    this->setStyleSheet(QString("QPlainTextEdit { border: 0px solid ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("textedit-border").foreground().color())+"; "+
                                        QString("border-right: 1px solid ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("textedit-border").foreground().color())+"; "+
                                        QString("color: ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").foreground().color())+"; "+
                                        QString("background-color: ")+
                                        ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats("normal").background().color())+
                                "; }");
    this->setCurrentCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    QTextCursor cur = this->textCursor();
    cur.setCharFormat(ConfigManager::Instance.getTextCharFormats("normal"));
    this->setTextCursor(cur);
    //this->widgetTextEdit->setCurrentFont(ConfigManager::Instance.getTextCharFormats("normal").font());


#ifdef OS_MAC
    if(ConfigManager::Instance.getTextCharFormats("normal").background().color().value()<100) // if it's a dark color
    {
        QPixmap whiteBeamPixmap(":/data/cursor/whiteBeam.png");
        QCursor whiteBeam(whiteBeamPixmap);
        this->viewport()->setCursor(whiteBeam);
    }
    else
    {
        this->viewport()->setCursor(Qt::IBeamCursor);
    }
#endif

}


void WidgetTextEdit::fold(int start, int end)
{
    _foldedLines.insert(start, end);
    for (int i = start + 1; i <= end; i++)
    {
        document()->findBlockByNumber(i).setVisible(false);
    }
    update();
    resizeEvent(new QResizeEvent(QSize(0, 0), size()));
    viewport()->update();
    _widgetLineNumber->update();
    ensureCursorVisible();
}
void WidgetTextEdit::unfold(int start)
{
    if (!_foldedLines.keys().contains(start)) return;
    int end = _foldedLines.value(start);
    _foldedLines.remove(start);
    int i=start+1;
    while (i<=end)
    {
        if (_foldedLines.keys().contains(i))
        {
            document()->findBlockByNumber(i).setVisible(true);
            i=_foldedLines.value(i);
        }
        else document()->findBlockByNumber(i).setVisible(true);
        i++;
    }
    update();
    resizeEvent(new QResizeEvent(QSize(0, 0), size()));
    viewport()->update();
    _widgetLineNumber->update();
    //ensureCursorVisible();
}

void WidgetTextEdit::goToSection(QString sectionName)
{
    int line = _textStruct->sectionNameToLine(sectionName);
    if(line != -1)
    {
        goToLine(line + 1);
    }
}

void WidgetTextEdit::onBlockCountChanged(int newBlockCount)
{
    int delta = newBlockCount - _lastBlockCount;
    QTextBlock block = textCursor().block();
    int currentline = block.blockNumber();
    int i = currentline-1;
    QList<int> start,end;

    while (block.isValid())
    {
        if (_foldedLines.keys().contains(i))
        {
            start.append(i+delta);
            end.append(_foldedLines[i]+delta);
            _foldedLines.remove(i);
        }
        i++;
        block = block.next();
    }
    for (int i = 0; i < start.count(); ++i)
    {
        _foldedLines.insert(start[i],end[i]);
    }
    _lastBlockCount = newBlockCount;
}
