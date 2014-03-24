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
#include <QPainter>
#include <QString>
#include <QBrush>
#include <QTextCharFormat>
#include <QTextBlock>
#include <QTextLayout>
#include <QScrollBar>
#include <QDebug>
#include <QPalette>

WidgetLineNumber::WidgetLineNumber(QWidget *parent) :
    QWidget(parent),
    widgetTextEdit(0),
    firstVisibleBlock(0),
    firstVisibleBlockTop(0),
    _currentLine(-1)
{
    this->scrollOffset = 0;


    /*this->setStyleSheet(QString("WidgetLineNumber { background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("line-number").background().color())+
                        "; }");
    qDebug()<<QString("background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("line-number").background().color())+
              ";";*/
}

void WidgetLineNumber::setWidgetTextEdit(WidgetTextEdit *widgetTextEdit)
{
    this->widgetTextEdit = widgetTextEdit;
    connect(this->widgetTextEdit,SIGNAL(updateFirstVisibleBlock(int,int)), this, SLOT(updateFirstVisibleBlock(int,int)));
    connect(this->widgetTextEdit,SIGNAL(updatedWithSameFirstVisibleBlock()), this, SLOT(update()));
    connect(this->widgetTextEdit,SIGNAL(lineCountChanged(int)), this, SLOT(updateWidth(int)));
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

    int width = ConfigManager::Instance.getTextCharFormats("line-number").font().pointSize();//fm.width("0") + 2;
    int ln = 1;
    while(lineCount >= 10)
    {
        lineCount /= 10;
        ++ln;
    }
    //qDebug()<<ln*width + 8;
    this->setMinimumWidth(ln*width + 13);

}

void WidgetLineNumber::paintEvent(QPaintEvent * /*event*/)
{
    if(!widgetTextEdit) return;

    //update info about the scroll position
    this->scrollOffset = -this->widgetTextEdit->verticalScrollBar()->value();


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

    int l;


    QPen defaultPen(ConfigManager::Instance.getTextCharFormats("line-number").foreground().color(),1);
    QPen currentLinePen(ConfigManager::Instance.getTextCharFormats("current-line-number").foreground().color(), 1);
    QPen blockRangePen(QColor(160,10,10),4);
    painter.setPen(defaultPen);

    int right = this->width()-5;
    int fontHeight = fm.height();
    l = this->firstVisibleBlock = this->widgetTextEdit->firstVisibleBlockNumber();
    this->scrollOffset = this->widgetTextEdit->contentOffsetTop();

    while(l < widgetTextEdit->document()->blockCount() && this->widgetTextEdit->blockTop(l) + this->scrollOffset < height())
    {
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
        painter.drawText(5,this->scrollOffset+this->widgetTextEdit->blockTop(l), right-9, fontHeight, Qt::AlignRight, QString::number(l+1));
        ++l;
    }

    // Block Range

}

void WidgetLineNumber::setBlockRange(int start, int end)
{
    _startBlock = start;
    _endBlock = end;
}

