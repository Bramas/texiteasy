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

#include "widgetpdfviewer.h"
#include "ui_widgetpdfviewer.h"
#include "widgetpdfdocument.h"
#include <QImage>
#include <QIcon>
#include <QDebug>

WidgetPdfViewer::WidgetPdfViewer(QWidget *parent) :
    QWidget(parent),
    //ui(new Ui::WidgetPdfViewer),
    _widgetPdfDocument(new WidgetPdfDocument(this))
{
    //ui->setupUi(this);

    _verticalLayout = new QVBoxLayout(this);
    this->setLayout(_verticalLayout);
    _verticalLayout->addWidget(_widgetPdfDocument);
    _verticalLayout->setSpacing(0);
    _verticalLayout->setMargin(0);
    _verticalLayout->setContentsMargins(0,0,0,0);
    /*
    this->ui->pushButtonZoomIn->setVisible(false);
    this->ui->pushButtonZoomOut->setVisible(false);
    this->ui->verticalLayout->addWidget(_widgetPdfDocument);
    this->ui->verticalLayout->setMargin(0);
    this->ui->verticalLayout->setContentsMargins(0,0,0,0);
    this->ui->verticalLayout->setSpacing(0);
    this->setContentsMargins(0,0,0,0);
    */
}

WidgetPdfViewer::~WidgetPdfViewer()
{
}

void WidgetPdfViewer::restorPdfDocumentParent()
{
    setWidgetPdfDocument(widgetPdfDocument());
}

void WidgetPdfViewer::setWidgetPdfDocument(WidgetPdfDocument * widgetPdfDocument)
{
    while(_verticalLayout->count())
    {
        _verticalLayout->removeItem(_verticalLayout->itemAt(0));
    }
    if(widgetPdfDocument == _widgetPdfDocument)
    {
        _widgetPdfDocument->setVisible(true);
    }
    else
    {
        _widgetPdfDocument->setVisible(false);
    }
    widgetPdfDocument->setParent(this);
    _verticalLayout->addWidget(widgetPdfDocument);
}

