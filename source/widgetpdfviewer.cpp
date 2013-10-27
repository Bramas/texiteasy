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
    ui(new Ui::WidgetPdfViewer),
    _widgetPdfDocument(new WidgetPdfDocument(this))
{
    ui->setupUi(this);

    this->ui->pushButtonZoomIn->setVisible(false);
    this->ui->pushButtonZoomOut->setVisible(false);
    this->ui->verticalLayout->removeWidget(this->ui->pushButtonZoomIn);
    this->ui->verticalLayout->removeWidget(this->ui->pushButtonZoomOut);
    this->ui->verticalLayout->addWidget(_widgetPdfDocument);
/*
     QPixmap zoomOut(":/icons/data/icons/zoom-out.png");
     QIcon buttonIconZoomOut(zoomOut);
     this->ui->pushButtonZoomOut->setIcon(buttonIconZoomOut);
     this->ui->pushButtonZoomOut->setIconSize(zoomOut.rect().size());

     QPixmap zoomIn(":/icons/data/icons/zoom-in.png");
     QIcon buttonIconZoomIn(zoomIn);
     this->ui->pushButtonZoomIn->setIcon(buttonIconZoomIn);
     this->ui->pushButtonZoomIn->setIconSize(zoomIn.rect().size());

     connect(this->ui->pushButtonZoomIn, SIGNAL(clicked()), this->_widgetPdfDocument, SLOT(zoomIn()));
     connect(this->ui->pushButtonZoomOut, SIGNAL(clicked()), this->_widgetPdfDocument, SLOT(zoomOut()));*/
}

WidgetPdfViewer::~WidgetPdfViewer()
{
    delete ui;
}

void WidgetPdfViewer::restorPdfDocumentParent()
{
    setWidgetPdfDocument(widgetPdfDocument());
}

void WidgetPdfViewer::setWidgetPdfDocument(WidgetPdfDocument * widgetPdfDocument)
{
    while(this->ui->verticalLayout->count())
    {
        this->ui->verticalLayout->removeItem(this->ui->verticalLayout->itemAt(0));
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
    this->ui->verticalLayout->addWidget(widgetPdfDocument);
}

