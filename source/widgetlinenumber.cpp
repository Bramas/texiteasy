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

#include "widgetlinenumber.h"
#include "widgettextedit.h"
#include "configmanager.h"
#include "filestructure.h"
#include <math.h>
#include <QPainter>
#include <QString>
#include <QBrush>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QTextLayout>
#include <QScrollBar>
#include <QDebug>
#include <QPalette>
#include <QStack>

const int FoldingWidth = 16;

WidgetLineNumber::WidgetLineNumber(QWidget *parent) :
    QWidget(parent),
    widgetTextEdit(0),
    firstVisibleBlock(0),
    firstVisibleBlockTop(0),
    _currentLine(-1)
{
    this->scrollOffset = 0;
    _isMouseOverUnfolding = false;
    _isMouseOverFolding = false;
    _foldableLineBegin = 0;
    _foldableLineEnd = 0;
    _unfoldableLine = 0;


    /*this->setStyleSheet(QString("WidgetLineNumber { background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("line-number").background().color())+
                        "; }");
    qDebug()<<QString("background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("line-number").background().color())+
              ";";*/
    this->setMouseTracking(true);
}

void WidgetLineNumber::setWidgetTextEdit(WidgetTextEdit *widgetTextEdit)
{
    this->widgetTextEdit = widgetTextEdit;
    connect(this->widgetTextEdit,SIGNAL(updateFirstVisibleBlock(int,int)), this, SLOT(updateFirstVisibleBlock(int,int)));
    connect(this->widgetTextEdit,SIGNAL(updatedWithSameFirstVisibleBlock()), this, SLOT(update()));
    connect(this->widgetTextEdit,SIGNAL(lineCountChanged(int)), this, SLOT(updateWidth(int)));
    connect(this->widgetTextEdit,SIGNAL(cursorPositionChanged()), this, SLOT(update()));
}


void WidgetLineNumber::updateFirstVisibleBlock(int block, int top)
{
    this->firstVisibleBlock = block;
    this->firstVisibleBlockTop = top;

    this->update();
    //qDebug()<<"first : "<<block<<"  "<< this->firstVisibleBlockTop;
}

void WidgetLineNumber::updateWidth(int lineCount)
{
    QFont font;
    font.setFamily(ConfigManager::Instance.getTextCharFormats("line-number").font().family());
    font.setPointSize(ConfigManager::Instance.getTextCharFormats("line-number").font().pointSize());
    QFontMetrics fm(font);

    _zeroWidth = fm.width("0");
    int ln = 1;
    while(lineCount >= 10)
    {
        lineCount /= 10;
        ++ln;
    }
    //qDebug()<<ln*width + 8;
    this->setMinimumWidth(ln*(_zeroWidth+2) + 8 + _zeroWidth + 5);

}

void WidgetLineNumber::paintEvent(QPaintEvent * /*event*/)
{
    if(!widgetTextEdit) return;

    //update info about the scroll position
    this->scrollOffset = -this->widgetTextEdit->verticalScrollBar()->value();

    QStack<const StructItem*> environmentPath = this->widgetTextEdit->textStruct()->environmentPath();
    _foldableLineBegin = environmentPath.top()->blockBeginNumber;
    _foldableLineEnd = environmentPath.top()->blockEndNumber;
    this->firstVisibleBlock = widgetTextEdit->firstVisibleBlockNumber();
    QPainter painter(this);

    QFont defaultFont;
    defaultFont.setFamily(ConfigManager::Instance.getTextCharFormats("line-number").font().family());
    defaultFont.setPointSize(ConfigManager::Instance.getTextCharFormats("line-number").font().pointSize());
    QFontMetrics fm(defaultFont);
    painter.setFont(defaultFont);

    QFont currentLineFont;
    currentLineFont.setFamily(ConfigManager::Instance.getTextCharFormats("line-number").font().family());
    currentLineFont.setPointSize(ConfigManager::Instance.getTextCharFormats("line-number").font().pointSize());
    currentLineFont.setWeight(QFont::Bold);

    QPen defaultPen(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color(),1);
    QPen currentLinePen(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color(), 1);
    QPen blockRangePen(QColor(160,10,10),4);
    painter.setPen(defaultPen);

    int right           = this->width() - 8 - _zeroWidth - 5;
    int fontHeight      = fm.height();
    int l               = this->firstVisibleBlock = this->widgetTextEdit->firstVisibleBlockNumber();
    this->scrollOffset  = this->widgetTextEdit->contentOffsetTop();

    _unfoldableLines.clear();
    QLine foldingLine(right + 10 + (_zeroWidth+1)/2, 0, right + 10 + (_zeroWidth+1)/2, height());
    if(environmentPath.top()->blockEndNumber < l)
    {
        foldingLine.setLine(-1,-1,-1,-1);
    }
    while(l < widgetTextEdit->document()->blockCount() && this->widgetTextEdit->blockTop(l) + this->scrollOffset < height())
    {
        if(!widgetTextEdit->document()->findBlockByNumber(l).isVisible())
        {
            ++l;
            continue;
        }
        if(l == _currentLine)
        {
            painter.setPen(currentLinePen);
            painter.setFont(currentLineFont);
        }
        else
        {
            painter.setPen(defaultPen);
            painter.setFont(defaultFont);
        }
        int top = this->scrollOffset+this->widgetTextEdit->blockTop(l) + 2;
        painter.drawText(5,top, right, fontHeight, Qt::AlignRight,   QString::number(l+1));


        if(widgetTextEdit->isFolded(l))
        {
            UnfoldableLine unfoldableLine;
            unfoldableLine.lineNumber = l;
            unfoldableLine.rect = QRect(right + 9, top + 2, _zeroWidth + 8, _zeroWidth + 8);
            unfoldableLine.isMouseOver = unfoldableLine.rect.contains(_lastMousePos);
            _unfoldableLines.append(unfoldableLine);
            if(unfoldableLine.isMouseOver)
            {
                painter.setPen(currentLinePen);
                painter.setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color())));
            }
            else
            {
                painter.setPen(defaultPen);
                painter.setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())));
            }
            painter.drawRect(QRectF(right + 10.0, top + 3.0 + _zeroWidth/2.0 - _zeroWidth/20.0, _zeroWidth, _zeroWidth/10.0));
            painter.drawRect(QRectF(right + 10.0 + _zeroWidth/2.0 - _zeroWidth/20.0, top + 3, _zeroWidth/10.0, _zeroWidth));

        }
        else
        {
            // Environement ranges
            if(l == environmentPath.top()->blockBeginNumber)
            {
                drawFoldingBegin(&painter, right + 10, top, _zeroWidth+1);
                foldingLine.setP1(QPoint(foldingLine.x1(), top + 3 + _zeroWidth+1));
            }
            else
            if(l == environmentPath.top()->blockEndNumber)
            {
                drawFoldingEnd(&painter, right + 10, top, _zeroWidth+1);
                foldingLine.setP2(QPoint(foldingLine.x2(), top + 3));
            }
        }
        ++l;
    }
    if(l > environmentPath.top()->blockBeginNumber && !widgetTextEdit->isFolded(environmentPath.top()->blockBeginNumber))
    {
        if(_isMouseOverFolding)
        {
            QColor c = QColor(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color());
            c.setAlpha(50);
            painter.setBrush(QBrush(c));
            //painter.drawLine(foldingLine);
            painter.drawRect(foldingLine.x1() - ceil(_zeroWidth/2) - 3,
                              foldingLine.y1() - _zeroWidth - 3,
                              2*ceil(_zeroWidth/2) + 6,
                              foldingLine.y2() - foldingLine.y1() + 2*_zeroWidth + 6
                              );
        }
        /*else
        {
            painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())));
        }
        // */
        _foldingHover.setRect(foldingLine.x1() - ceil(_zeroWidth/2) - 1,
                              foldingLine.y1() - _zeroWidth,
                              2*ceil(_zeroWidth/2) + 2,
                              foldingLine.y2() - foldingLine.y1() + 2*_zeroWidth
                              );
    }
    else
    {
        _foldingHover.setRect(-1, -1, 0, 0);
    }

}
void WidgetLineNumber::drawFoldingBegin(QPainter* painter, int right, int top, int width)
{
    if(_isMouseOverFolding)
    {
        painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color())));
    }
    else
    {
        painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())));
    }
    painter->drawEllipse(right, top + 3 , width, width);
    QPoint p[3];
    p[0].setX(right + width/2);
    p[0].setY(top + 3 + width);

    p[1].setX(right + ceil(width/2.0 - sqrt(3)*width/4.0));
    p[1].setY(top + 3 + width/4);
    p[2].setX(right + floor(width/2.0 + sqrt(3)*width/4.0));
    p[2].setY(p[1].y());
    painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").background().color())));
    painter->drawConvexPolygon(p, 3);
}

void WidgetLineNumber::drawFoldingEnd(QPainter* painter, int right, int top, int width)
{
    if(_isMouseOverFolding)
    {
        painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color())));
    }
    else
    {
        painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color())));
    }
    painter->drawEllipse(right, top + 3 , width, width);
    QPoint p[3];
    p[0].setX(right + width/2);
    p[0].setY(top + 3);

    p[1].setX(right + ceil(width/2.0 - sqrt(3)*width/4.0));
    p[1].setY(top + 3 + width -  width/4);
    p[2].setX(right + floor(width/2.0 + sqrt(3)*width/4.0));
    p[2].setY(p[1].y());
    painter->setBrush(QBrush(QColor(ConfigManager::Instance.getTextCharFormats("line-number").background().color())));
    painter->drawConvexPolygon(p, 3);
}

void WidgetLineNumber::mouseMoveEvent(QMouseEvent *event)
{
    _lastMousePos = event->pos();
    foreach(UnfoldableLine unfoldable, _unfoldableLines)
    {
        if((unfoldable.rect.contains(event->pos()) && !unfoldable.isMouseOver)
                || (!unfoldable.rect.contains(event->pos()) && unfoldable.isMouseOver))
        {
            _isMouseOverFolding = false;
            update();
            return;
        }
        if(unfoldable.rect.contains(event->pos()))
        {
            return;
        }
    }

    if(_foldingHover.contains(event->pos()))
    {
        if(!_isMouseOverFolding)
        {
            _isMouseOverFolding = true;
            update();
        }
        else
        {
            _isMouseOverFolding = true;
        }
    }
    else
    {
        if(_isMouseOverFolding)
        {
            _isMouseOverFolding = false;
            update();
        }
        else
        {
            _isMouseOverFolding = false;
        }

    }
}
void WidgetLineNumber::leaveEvent(QEvent *)
{
    _lastMousePos = QPoint(-1, -1);
    if(_isMouseOverFolding)
    {
        _isMouseOverFolding = false;
        update();
    }
    else
    {
        _isMouseOverFolding = false;
    }
    foreach(UnfoldableLine unfoldable, _unfoldableLines)
    {
        if(unfoldable.isMouseOver)
        {
            update();
        }
    }
}
void WidgetLineNumber::mousePressEvent(QMouseEvent * event)
{
    foreach(UnfoldableLine unfoldable, _unfoldableLines)
    {
        if(unfoldable.rect.contains(event->pos()))
        {
            _isMouseOverFolding = false;
            _foldingHover.setRect(-1, -1, 0, 0);
            widgetTextEdit->unfold(unfoldable.lineNumber);
            return;
        }
    }
    if(_foldingHover.contains(event->pos()))
    {
        widgetTextEdit->fold(_foldableLineBegin, _foldableLineEnd);
        return;
    }
}

void WidgetLineNumber::setBlockRange(int start, int end)
{
    _startBlock = start;
    _endBlock = end;
}

