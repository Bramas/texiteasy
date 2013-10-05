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

#ifndef WIDGETTEXTEDIT_H
#define WIDGETTEXTEDIT_H

#include <QPlainTextEdit>
#include <QTextBlock>
#include <QTextLayout>
#include <QMutex>
#include <QAbstractTextDocumentLayout>
#include "file.h"


#define WIDGET_TEXT_EDIT_PARENT_CLASS QPlainTextEdit

class FileStructure;
class SyntaxHighlighter;
class CompletionEngine;
class WidgetInsertCommand;
class WidgetLineNumber;

struct BlockInfo
{
    int top;
    int height;
    int position;
    int leftMargin;
    BlockInfo() : top(0), height(0), position(0), leftMargin(0) {}
};


class WidgetTextEdit : public WIDGET_TEXT_EDIT_PARENT_CLASS
{
    Q_OBJECT
public:
    explicit WidgetTextEdit(QWidget *parent);
    int blockHeight(int blockCount) { return blockHeight(this->document()->findBlockByNumber(blockCount)); }
    int blockHeight(const QTextBlock &textBlock) { return this->blockGeometry(textBlock).height(); }
    int blockWidth(int blockCount) { return blockWidth(this->document()->findBlockByNumber(blockCount)); }
    int blockWidth(const QTextBlock &textBlock) { return this->blockGeometry(textBlock).width(); }
    int blockTop(int blockCount) { return blockTop(this->document()->findBlockByNumber(blockCount)); }
    int blockTop(const QTextBlock &textBlock) { return this->blockGeometry(textBlock).top(); }
    int blockBottom(int blockCount) { return blockBottom(this->document()->findBlockByNumber(blockCount)); }
    int blockBottom(const QTextBlock &textBlock) { return this->blockGeometry(textBlock).bottom(); }

    QRectF blockGeometry(const QTextBlock &textBlock) { return this->blockBoundingGeometry(textBlock); }
    int contentOffsetTop() { return this->contentOffset().y(); }


    int textHeight() { return this->blockBottom(this->document()->end()); }
    File * getCurrentFile() { return this->currentFile; }
    void setText(const QString &text);
    void insertText(const QString &text);
    int firstVisibleBlockNumber() { return this->firstVisibleBlock().blockNumber(); } // return this->firstVisibleBlock; }

    bool isCursorVisible();
    void setSyntaxHighlighter(SyntaxHighlighter * syntaxHighlighter) { this->_syntaxHighlighter = syntaxHighlighter; }
    void setWidgetLineNumber(WidgetLineNumber * widgetLineNumber) { this->_widgetLineNumber = widgetLineNumber; }
    void displayWidgetInsertCommand();

    int centerBlockNumber();
    void highlightSyncedLine(int line);

signals:
    void updateFirstVisibleBlock(int,int);
    void updatedWithSameFirstVisibleBlock();
    void setBlockRange(int,int);
    void lineCountChanged(int);
    void cursorPositionChanged(int, int);
public slots:
    void scrollTo(int);
    void updateIndentation(void);
    void onCursorPositionChange(void);
    void matchCommand();
    void matchAll();
    void setFocus() { WIDGET_TEXT_EDIT_PARENT_CLASS::setFocus(); }
    void setFocus(QKeyEvent * event) { WIDGET_TEXT_EDIT_PARENT_CLASS::setFocus(); this->keyPressEvent(event); }
    void insertPlainText(const QString &text);
    void goToLine(int line, QString stringSelected = QString());
protected:
    void insertFromMimeData(const QMimeData * source);
    void mousePressEvent(QMouseEvent *e) {
        _multipleEdit.clear();
        WIDGET_TEXT_EDIT_PARENT_CLASS::mousePressEvent(e);
    }

private:
    void initIndentation(void);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent * event);
    void wheelEvent(QWheelEvent * event);
    void highlightCurrentLine(void);
    bool selectNextArgument(void);

    void setBlockLeftMargin(const QTextBlock & textBlock, int leftMargin);

    void matchPar();
    bool matchLeftPar(QTextBlock currentBlock, int index, int numLeftPar );
    bool matchRightPar(QTextBlock currentBlock, int index, int numRightPar);
    void createParSelection(int pos );
    void matchLat();
    int matchLeftLat(QTextBlock currentBlock, int index, int numLeftLat, int bpos);
    int matchRightLat(QTextBlock currentBlock, int index, int numLeftLat, int bpos);
    void createLatSelection(int start, int end);

    QList<QTextCursor> _multipleEdit;

    CompletionEngine * _completionEngine;
    File * currentFile;
    FileStructure * fileStructure;
    QMutex _formatMutex;
    bool _indentationInited;
    QMutex _indentationMutex;
    int _lineCount;
    SyntaxHighlighter * _syntaxHighlighter;
    bool updatingIndentation;
    WidgetInsertCommand * _widgetInsertCommand;
    WidgetLineNumber * _widgetLineNumber;


};

#endif // WIDGETTEXTEDIT_H
