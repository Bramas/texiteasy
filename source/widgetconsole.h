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

#ifndef WIDGETCONSOLE_H
#define WIDGETCONSOLE_H

#include <QScrollArea>
#include <QPlainTextEdit>

#include "ipane.h"

class Builder;
class WidgetFile;

class WidgetConsole : public QObject, public IPane
{
    Q_OBJECT
public:
    explicit WidgetConsole(WidgetFile * widgetFile);
    ~WidgetConsole();



    QString statusbarText() { return "Console"; }
    QWidget * paneWidget();
    QObject * getQObject();
    QAction * action() { return _action; }
    
signals:
    void requestLine(int);
public slots:
    void expand(void);
    void collapsed(void);
    void onError(void);
    void onSuccess(void);
    void setOutput(QString newText);

    void openMyPane();
    void closeMyPane();

protected:
    //void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);

private slots:
    void updateBuilder();

private:

    bool _collapsed;
    int _height;
    Builder * _builder;
    QPlainTextEdit * _mainWidget;
    WidgetFile * _widgetFile;
    QAction * _action;
    
};

#endif // WIDGETCONSOLE_H
