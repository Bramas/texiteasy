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

#ifndef WIDGETSCROLLER_H
#define WIDGETSCROLLER_H

#include <QWidget>
#include <QRectF>

class WidgetTextEdit;
class WidgetScroller : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetScroller(QWidget *parent = 0);
    void setParent(QWidget *parent) {QWidget::setParent(parent); this->parent = parent; }

    void setWidgetTextEdit(WidgetTextEdit * widgetTextEdit) { this->widgetTextEdit = widgetTextEdit; }
    
signals:
    void changed(int);
public slots:
    void updateText(void);

private:
    QRect boudingRect;
    bool mousePressed;
    int mousePressedAt;
    QRectF overlayRect;
    QWidget *parent;
    int scrollOffset;
    WidgetTextEdit * widgetTextEdit;

    void paintEvent(QPaintEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};

#endif // WIDGETSCROLLER_H
