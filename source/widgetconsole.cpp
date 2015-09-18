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

#include "widgetconsole.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QDebug>
#include <QSizePolicy>
#include <QTextBlock>
#include <QScrollBar>
#include <QAction>
#include "widgetfile.h"
#include "builder.h"
#include "filemanager.h"

WidgetConsole::WidgetConsole(WidgetFile *widgetFile) :
    _builder(0),
    _mainWidget(new QPlainTextEdit(0)),
    _widgetFile(widgetFile)
{
    _action = new QAction(statusbarText(), 0);
    _action->setCheckable(true);
    _height = 100;
    _collapsed = true;
    _mainWidget->setReadOnly(true);
    this->updateBuilder();
    connect(_widgetFile, SIGNAL(opened()), this, SLOT(updateBuilder()));
}

WidgetConsole::~WidgetConsole()
{
#ifdef DEBUG_DESTRUCTOR
    qDebug()<<"delete WidgetConsole";
#endif
}


QWidget * WidgetConsole::paneWidget()
{
    return _mainWidget;
}

QObject * WidgetConsole::getQObject()
{
    return this;
}

void WidgetConsole::updateBuilder()
{
    this->_builder = _widgetFile->file()->builder();
    if(!this->_builder || !FileManager::Instance.currentWidgetFile())
    {
        return;
    }

    connect(_builder, SIGNAL(error()),this, SLOT(onError()));
    connect(_builder, SIGNAL(success()),this, SLOT(onSuccess()));
    connect(_builder, SIGNAL(started()), _mainWidget, SLOT(clear()));
    connect(_builder, SIGNAL(started()), this, SLOT(openMyPane()));
    connect(_builder, SIGNAL(success()), this, SLOT(closeMyPane()));
    connect(_builder, SIGNAL(outputUpdated(QString)), this, SLOT(setOutput(QString)));
}

void WidgetConsole::openMyPane()
{
    if(FileManager::Instance.currentWidgetFile())
    {
        FileManager::Instance.currentWidgetFile()->openPane(this);
    }
}
void WidgetConsole::closeMyPane()
{
    if(FileManager::Instance.currentWidgetFile())
    {
        FileManager::Instance.currentWidgetFile()->closePane(this);
    }
}

void WidgetConsole::mousePressEvent(QMouseEvent *event)
{
    /*QPlainTextEdit::mousePressEvent(event);
    if(event->modifiers() == Qt::CTRL)
    {
        //this->setCursor(Qt::PointingHandCursor);
        //qDebug()<<this->textCursor().blockNumber();
        QString text = this->textCursor().block().text();
        if(text.length() && !text.left(2).compare("l."))
        {
            int firstSpace = text.indexOf(" ");
            int lineNumber = text.mid(2,firstSpace-2).toInt();
            emit requestLine(lineNumber);
        }
    }*/
}

void WidgetConsole::expand()
{
    if(_collapsed)
    {
        _collapsed = false;
        //this->setMaximumHeight(_height);
        //this->setMinimumHeight(_height);
    }
}

void WidgetConsole::collapsed()
{
    if(!_collapsed)
    {
        _collapsed = true;

        //this->setMaximumHeight(0);
        //this->setMinimumHeight(0);
    }
}
void WidgetConsole::onError()
{
    //this->setPlainText(this->_builder->simpleOutput().join("\n"));
    this->expand();
}
void WidgetConsole::onSuccess()
{
    //this->setPlainText(this->_builder->simpleOutput().join("\n"));
    this->collapsed();
}

void WidgetConsole::setOutput(QString newText)
{
    bool atEnd = false;
    if(_mainWidget->verticalScrollBar()->value() == _mainWidget->verticalScrollBar()->maximum())
    {
        atEnd = true;
    }
    _mainWidget->setPlainText(newText);
    if(atEnd)
    {
        //qDebug()<<this->verticalScrollBar()->maximum();
        _mainWidget->verticalScrollBar()->setValue(_mainWidget->verticalScrollBar()->maximum());
        //this->scroll();
    }
}
