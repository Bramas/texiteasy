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
 *   along with texiteasy.  If not, see <http://www.gnu.org/licenses/>.    *                         *
 *                                                                         *
 ***************************************************************************/


#include "widgettooltip.h"
#include <QLabel>
#include <QRect>

WidgetTooltip::WidgetTooltip(QWidget *parent) :
    QWidget(parent)
{
    this->setStyleSheet("QWidget{ background-color:#dddddd; }");
}

void WidgetTooltip::setText(QString text)
{
    QLabel * label = new QLabel(text,this);

    label->setWordWrap(true);
    QRect geo(0, 0, 200, 60);
    label->setGeometry(geo);

    geo = this->geometry();
    this->setGeometry(geo.x(), geo.y(), label->width()+10, label->height()+10);
}

void WidgetTooltip::setTopLeft(int left, int top)
{
    QRect geo = this->geometry();
    this->setGeometry(left, top, geo.width(), geo.height());
}
