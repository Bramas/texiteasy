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
    firstVisibleBlockTop(0)
{
    this->scrollOffset = 0;


    /*this->setStyleSheet(QString("WidgetLineNumber { background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("linenumber").background().color())+
                        "; }");
    qDebug()<<QString("background-color: black")+//ConfigManager::Instance.colorToString(ConfigManager::Instance.getTextCharFormats()->value("linenumber").background().color())+
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
    font.setFamily(ConfigManager::Instance.getTextCharFormats("linenumber").font().family());
    font.setPointSize(ConfigManager::Instance.getTextCharFormats("linenumber").font().pointSize());
    QFontMetrics fm(font);

    int width = ConfigManager::Instance.getTextCharFormats("linenumber").font().pointSize();//fm.width("0") + 2;
    int ln = 1;
    while(lineCount >= 10)
    {
        lineCount /= 10;
        ++ln;
    }
    //qDebug()<<ln*width + 8;
    this->setMinimumWidth(ln*width + 8);

}

void WidgetLineNumber::paintEvent(QPaintEvent * /*event*/)
{
    if(!widgetTextEdit) return;

    //update info about the scroll position
    this->scrollOffset = -this->widgetTextEdit->verticalScrollBar()->value();


    QPainter painter(this);

    QFont font;
    font.setFamily(ConfigManager::Instance.getTextCharFormats("linenumber").font().family());
    font.setPointSize(ConfigManager::Instance.getTextCharFormats("linenumber").font().pointSize());
    painter.setFont(font);
    QFontMetrics fm(font);
    painter.setFont(font);

    QTextBlock textBlock = widgetTextEdit->document()->findBlockByNumber(this->firstVisibleBlock);

    //qDebug()<<"line";
    int l;

   /* BlockInfo * blocksInfo = this->widgetTextEdit->getBlocksInfo();
    for(int i=0; i < this->widgetTextEdit->document()->blockCount(); ++i)
    {
        if(blocksInfo[i].top > this->widgetTextEdit->verticalScrollBar()->value())
        {
            qDebug()<<i;
        }
    }*/
    QPen defaultPen(ConfigManager::Instance.getTextCharFormats("linenumber").foreground().color(),1);
    QPen blockRangePen(QColor(160,10,10),4);
    painter.setPen(defaultPen);

    int right = this->width()-5;
    int fontHeight = fm.height();
    int cumulatedPosition = this->firstVisibleBlockTop;
    //qDebug()<<this->firstVisibleBlock;
    if(this->firstVisibleBlock <= 1)
    {
        painter.drawText(0, this->scrollOffset+5, right-9, fontHeight, Qt::AlignRight, QString::number(1));
    }
    for(l = this->firstVisibleBlock+1; l <= widgetTextEdit->document()->blockCount(); ++l)
    {
        //qDebug()<<l;//<<"  "<<this->widgetTextEdit->blockGeometry(textBlock).bottom();
        //if(this->widgetTextEdit->blockBottom(textBlock) < this->widgetTextEdit->verticalScrollBar()->value())//  !textBlock.isVisible())
        //{
        //    textBlock = textBlock.next();
        //    continue;
        //}
        if(!textBlock.isValid())
        {
            break;
        }
        painter.drawText(0,this->scrollOffset+cumulatedPosition+5, right-9, fontHeight, Qt::AlignRight, QString::number(l));
        if(l == _startBlock + 1)
        {
            painter.setPen(blockRangePen);
            painter.drawLine(right,this->scrollOffset+cumulatedPosition+15,right,this->scrollOffset+cumulatedPosition+widgetTextEdit->blockHeight(textBlock));
            painter.drawRect(right-3,this->scrollOffset+cumulatedPosition+10,6,6);
            painter.setPen(defaultPen);
        }
        if(l > _startBlock + 1 && l < _endBlock + 2)
        {
            painter.setPen(blockRangePen);
            painter.drawLine(right, this->scrollOffset+cumulatedPosition, right, this->scrollOffset+cumulatedPosition+widgetTextEdit->blockHeight(textBlock));
            painter.setPen(defaultPen);
        }

        cumulatedPosition += widgetTextEdit->blockHeight(textBlock);
        textBlock = textBlock.next();
    }

    // Block Range

}

void WidgetLineNumber::setBlockRange(int start, int end)
{
    _startBlock = start;
    _endBlock = end;
}

