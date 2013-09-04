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

#include "widgetscroller.h"
#include "widgettextedit.h"
#include <QPainter>
#include <QScrollBar>
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QDebug>

WidgetScroller::WidgetScroller(QWidget *parent) :
    QWidget(parent),
    parent(parent),
    widgetTextEdit(0)
{
    this->scrollOffset = 0;
    this->setGeometry(QRect(0,0,100,height()));
    this->setMouseTracking(true);
    mousePressed = false;
    this->boudingRect = QRect(0,0,100,100);
}

void WidgetScroller::updateText(void)
{
    QFontMetrics fm(QFont("Consolas",2));
    this->boudingRect = fm.boundingRect(0,0,this->widgetTextEdit->width()/4,99999999,Qt::AlignLeft,this->widgetTextEdit->document()->toPlainText());
}

void WidgetScroller::paintEvent(QPaintEvent * /*event*/)
{
    if(!widgetTextEdit) return;

    //update info about the scroll position
    this->scrollOffset = -this->widgetTextEdit->verticalScrollBar()->value();
    QFontMetrics fm(QFont("Consolas",2));

    this->overlayRect = QRectF(0,-this->scrollOffset/2.0,this->widgetTextEdit->width()/4,height()*this->widgetTextEdit->height()/(this->widgetTextEdit->textHeight()+1));

    QPainter painter(this);

    this->setGeometry(QRect(this->geometry().left(),this->geometry().top(),this->widgetTextEdit->width()/4,height()));
    painter.setFont(QFont("Consolas",2));

    painter.drawText(QRect(0, this->scrollOffset * (boudingRect.height()-height())/(height()-this->overlayRect.height()),this->widgetTextEdit->width()/4,99999999),Qt::TextWordWrap,this->widgetTextEdit->document()->toPlainText());

    painter.setBrush(QBrush(QColor(0,0,0,100)));

    painter.drawRect(this->overlayRect);
}
void WidgetScroller::mousePressEvent(QMouseEvent *event)
{
    mousePressed = true;
    mousePressedAt = this->scrollOffset+event->pos().y()*2;
    if(event->pos().y() < this->overlayRect.y() || event->pos().y() > this->overlayRect.y() + this->overlayRect.height())
    {
        mousePressedAt=0;
        qDebug()<<"outside "<<event->pos().y();
        emit changed(event->pos().y()*2);
    }
}
void WidgetScroller::mouseReleaseEvent(QMouseEvent */*event*/)
{
    mousePressed = false;
}

void WidgetScroller::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePressed)
    {
        emit changed(event->pos().y()*2-mousePressedAt);
    }
}
