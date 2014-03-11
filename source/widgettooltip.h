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


#ifndef WIDGETTOOLTIP_H
#define WIDGETTOOLTIP_H

#include <QWidget>
#include <QTextEdit>

class WidgetTooltip : public QTextEdit
{
    Q_OBJECT
public:
    explicit WidgetTooltip(QWidget *parent, QWidget * completionEngine);
    void setTopLeft(int left, int top);

signals:

public slots:
    void setText(QString text);
    void setFocus();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    QWidget * _completionEngine;

};

#endif // WIDGETTOOLTIP_H
