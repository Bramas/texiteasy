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

#include "widgetpdfdocument.h"
#include "widgettextedit.h"
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QBrush>
#include <QRectF>
#include <QColor>
#include "file.h"



#define SYNCTEX_GZ_EXT ".synctex.gz"
#define SYNCTEX_EXT ".synctex"
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))



QImage * WidgetPdfDocument::EmptyImage = new QImage();
int WidgetPdfDocument::PageMargin = 20;

WidgetPdfDocument::WidgetPdfDocument(QWidget *parent) :
    QWidget(parent),
    _document(0),
    _documentHeight(0),
    _file(0),
    _loadedPages(0),
    _mousePressed(false),
    _pages(0),
    scanner(NULL),
    _scroll(new QScrollBar(Qt::Vertical, this)),
    _widgetTextEdit(0),
    _zoom(1)

{
    this->setMouseTracking(true);
    this->setCursor(Qt::OpenHandCursor);
    connect(&this->_timer, SIGNAL(timeout()),this, SLOT(update()));

    _scroll->setGeometry(this->width()-20,0,20,200);
    _scroll->setRange(0,1);

    connect(_scroll, SIGNAL(valueChanged(int)), this, SLOT(onScroll(int)));
    connect(this, SIGNAL(translated(int)), _scroll, SLOT(setValue(int)));

}

void WidgetPdfDocument::paintEvent(QPaintEvent *)
{
    if(!this->_document)
    {
        return;
    }
    QPainter painter(this);
    painter.translate(this->_painterTranslate);
    painter.setBrush(QBrush(QColor(0,0,0,50)));
    painter.setPen(QPen(QColor(0,0,0,0)));
    QImage * image;
    int cumulatedTop=0;
    for(int i = 0; i < this->_document->numPages(); ++i)
    {
        if(cumulatedTop + _document->page(i)->pageSize().height()*_zoom < -this->_painterTranslate.y())
        {
            //qDebug()<<cumulatedTop<<","<<_document->page(i)->pageSize().height();
            cumulatedTop += _document->page(i)->pageSize().height()*_zoom+WidgetPdfDocument::PageMargin;
            continue;
        }
        image = this->page(i);
        painter.drawImage(0,cumulatedTop,*image);
        if(i == _syncPage+1)
        {
            if(_lastUpdate.elapsed()<1200)
            {
                painter.drawRect(0, cumulatedTop, this->width()*_zoom+1, this->height());
            }
        }
        if(i == _syncPage)
        {
            if(_lastUpdate.elapsed()<1200)
            {
                painter.setBrush(QBrush(QColor(0,0,0,min(50,(750-_lastUpdate.elapsed())*(2250-_lastUpdate.elapsed())*50/(750*750) + 50))));
                //qDebug()<<"t "<<_lastUpdate.elapsed();
                painter.drawRect(0, cumulatedTop - this->height(), this->width()*_zoom+1, this->height() + _syncRect.y() * _zoom);
                painter.drawRect(0, _syncRect.y()*_zoom + cumulatedTop + _syncRect.height()*_zoom, this->width()*_zoom+1, this->height());
                painter.drawRect(0, _syncRect.y()*_zoom + cumulatedTop, _syncRect.x()*_zoom, _syncRect.height()*_zoom);
                painter.drawRect((_syncRect.x() + _syncRect.width())*_zoom, _syncRect.y()*_zoom + cumulatedTop, width(), _syncRect.height()*_zoom);
            }
            else
            {
                this->_timer.stop();
            }
            //painter.drawLine(_syncRect.x()*_zoom-50,_syncRect.y()*_zoom + cumulatedTop,_syncRect.x()*_zoom-20,_syncRect.y()*_zoom + cumulatedTop);
            //painter.drawLine(_syncRect.x()*_zoom-50,_syncRect.y()*_zoom + cumulatedTop,_syncRect.x()*_zoom-50,_syncRect.y()*_zoom + 20 + cumulatedTop);
        }

        int pageHeight = _document->page(i)->pageSize().height()*_zoom;
        //Display page number:
        QString pageNumString = QString::number(i+1)+"/"+QString::number(_document->numPages());
        QFontMetrics fm(painter.font());
        int widthPageNumString = fm.width(pageNumString);

        painter.setBrush(QBrush(QColor(0,0,0,50)));
        painter.setPen(QPen(Qt::transparent));
        QRectF pageNumberDisp(max(0,-_painterTranslate.x()), min(cumulatedTop+pageHeight-22,max(-_painterTranslate.y(),cumulatedTop)),widthPageNumString+8,22);
        painter.drawRect(pageNumberDisp);
        painter.setPen(QPen(Qt::white));
        painter.drawText(pageNumberDisp.translated(4,4), pageNumString);


        cumulatedTop += pageHeight+WidgetPdfDocument::PageMargin;
        if(cumulatedTop > this->height() - this->_painterTranslate.y())
        {
            break;
        }
    }


}

void WidgetPdfDocument::initDocument()
{
    if(!_file)
    {
        return;
    }

    if(_pages)
    {
        this->refreshPages();
        delete _pages;
    }

    if(_document)
    {
        delete _document;
        _document = 0;
    }
    if(!QFile::exists(_file->getPdfFilename()))
    {
        return;
    }
    _document = Poppler::Document::load(_file->getPdfFilename());//PdfDocument::load(_file->getFilename());//new PdfDocument(_file->getFilename());

    if(!_document || _document->isLocked())
    {
        delete _document;
        _document = 0;
        return;
    }

    _document->setRenderHint(Poppler::Document::Antialiasing);
    _document->setRenderHint(Poppler::Document::TextAntialiasing);


    _pages = new QImage*[_document->numPages()];

    if(_loadedPages)
    {
        delete _loadedPages;
    }
    _loadedPages = new bool[_document->numPages()];
    for(int idx = 0; idx < _document->numPages(); ++idx)
    {
        _loadedPages[idx] = false;
    }

    this->initLinks();
    this->initScroll();

    QFileInfo fileInfo(this->_file->getFilename());
    QString syncFile = fileInfo.canonicalPath() + QDir().separator() + fileInfo.baseName();
    if(QFile::exists(syncFile+".synctex.gz"))
    {
        scanner = synctex_scanner_new_with_output_file(syncFile.toUtf8().data(), NULL, 1);
        if( scanner == NULL )
        {
            scanner = synctex_scanner_new_with_output_file(syncFile.toLatin1().data(), NULL, 1);
        }
        if( scanner == NULL )
        {
            qDebug()<<"scanner is NULL, cannot open "<<syncFile+".synctex.gz"<<" -> Maybe some special character that make it fails?";
        }
        jumpToPdfFromSource();
        update();
    }
    else
    {
        qDebug()<<"Sync file does not exists : "<<syncFile+".synctex.gz";
    }
}

void WidgetPdfDocument::initScroll()
{
    if(!_document->numPages())
    {
        return;
    }

    int height = -WidgetPdfDocument::PageMargin;
    for(int page_idx = 0; page_idx < _document->numPages(); ++page_idx)
    {
        height += _document->page(page_idx)->pageSize().height();
    }
    _documentHeight = height;
    this->_scroll->setRange(0,this->documentHeight() - this->height()+30);
}



void WidgetPdfDocument::initLinks()
{
    _links.clear();

    QRectF linkArea;
    qreal height;
    qreal width;
    qreal top;
    qreal left;
    int cumulatedTop = 0;
    for(int page_idx = 0; page_idx < _document->numPages(); ++page_idx)
    {
        if(_document->page(page_idx)->links().count())
        {
            foreach(const Poppler::Link * popLink, _document->page(page_idx)->links())
            {
                if(popLink->linkType() == Poppler::Link::Goto)
                {
                    Link link;
                    linkArea = popLink->linkArea();
                    height = _document->page(page_idx)->pageSize().height()*linkArea.height()*_zoom;
                    width = _document->page(page_idx)->pageSize().width()*linkArea.width()*_zoom;
                    top = _document->page(page_idx)->pageSize().height()*linkArea.top()*_zoom+cumulatedTop;
                    left = _document->page(page_idx)->pageSize().width()*linkArea.left()*_zoom;
                    link.rectangle = QRectF(left,top,width,height);
                    link.destination = new Poppler::LinkDestination(dynamic_cast<const Poppler::LinkGoto*>(popLink)->destination());
                    _links.append(link);
                }
            }
        }
        cumulatedTop += _document->page(page_idx)->pageSize().height()*_zoom+WidgetPdfDocument::PageMargin;
    }

    /*if(linkAreaAbsolute.contains(this->cursor().pos()))
    {
        this->setCursor(QCursor(Qt::PointingHandCursor));
    }*/
}

void WidgetPdfDocument::resizeEvent(QResizeEvent *)
{
    _scroll->setGeometry(this->width()-20,0,20,this->height());
    _scroll->setPageStep(this->height() / _zoom);

    this->boundPainterTranslation();

}
void WidgetPdfDocument::onScroll(int value)
{
    this->_painterTranslate.setY(-value);
    update();
}

QImage * WidgetPdfDocument::page(int page)
{
    if(!_pages || page < 0 || page >= _document->numPages())
    {
        return WidgetPdfDocument::EmptyImage;
    }
    if(_loadedPages[page])
    {
        return _pages[page];
    }
    _loadedPages[page] = true;
    return  _pages[page] = new QImage(this->_document->page(page)->renderToImage(this->_zoom*72.0,this->_zoom*72.0));

}
void WidgetPdfDocument::goToPage(int page, int top, int height)
{
    if(!_file) return;

    page = min(page,this->_document->numPages()-1);

    int cumulatedTop = 0;
    int i = 0;
    for(i = 0; i < page; ++i)
    {
        cumulatedTop += _document->page(i)->pageSize().height()*_zoom+WidgetPdfDocument::PageMargin;
    }
    //qDebug()<<(this->height());
    //qDebug()<<(-this->_painterTranslate.y() + this->height())<<"  <  "<<(cumulatedTop + height * _zoom)<<" | "<<(-this->_painterTranslate.y())<<" > "<<cumulatedTop + top ;
    if(-this->_painterTranslate.y() + this->height() < cumulatedTop + top*_zoom + height * _zoom || -this->_painterTranslate.y() > cumulatedTop + top*_zoom )
    {
        this->_painterTranslate.setY(-cumulatedTop-top*_zoom-height*_zoom/2+this->height()/2);
        emit translated( - _painterTranslate.y());
    }

    update();
}

void WidgetPdfDocument::refreshPages()
{
    for(int idx = 0; idx < this->_document->numPages(); ++idx)
    {
        if(_loadedPages[idx])
        {
            delete _pages[idx];
            _loadedPages[idx] = false;
        }
    }

}
void WidgetPdfDocument::checkLinksOver(const QPointF &pos)
{
    QPointF absolutePos = pos - this->_painterTranslate;
    //absolutePos *= _zoom;
    this->setCursor(Qt::ArrowCursor);
    foreach(const Link &link, _links)
    {
        if(link.rectangle.contains(absolutePos))
        {
            this->setCursor(Qt::PointingHandCursor);
            break;
        }
    }
}
bool WidgetPdfDocument::checkLinksPress(const QPointF &pos)
{
    QPointF absolutePos = pos - this->_painterTranslate;
    foreach(const Link &link, _links)
    {
        if(link.rectangle.contains(absolutePos))
        {
            int pageNumber = link.destination->pageNumber() - 1;
            int top = link.destination->top()*_document->page(pageNumber)->pageSize().height();
            int left = link.destination->left()*_document->page(pageNumber)->pageSize().width();
            int bottom = link.destination->bottom()*_document->page(pageNumber)->pageSize().height();
            int right = link.destination->right()*_document->page(pageNumber)->pageSize().width();
            this->goToPage(pageNumber, top);

            _syncPage = pageNumber;
            _syncRect = QRectF(left, top, right-left, bottom-top);
            _lastUpdate.start();
            _timer.start(1);

            return true;
        }
    }
    return false;
}

void WidgetPdfDocument::mousePressEvent(QMouseEvent * event)
{
    if(this->checkLinksPress(event->posF()))
    {
        return;
    }
    if(event->modifiers() == Qt::ControlModifier)
    {
        this->jumpToEditorFromAbsolutePos(event->posF());
        return;
    }
    this->_pressAt = event->pos();
    this->_painterTranslateWhenMousePressed = this->_painterTranslate;
    this->_mousePressed = true;
    this->setCursor(Qt::ClosedHandCursor);
}
void WidgetPdfDocument::mouseReleaseEvent(QMouseEvent * /*event*/)
{
    this->_mousePressed = false;
    this->setCursor(Qt::OpenHandCursor);
    update();
}
void WidgetPdfDocument::wheelEvent(QWheelEvent * event)
{
    if(event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::ShiftModifier))
    {
        qreal factor =  event->delta() > 0 ? 1.1 : 0.90;
        this->zoom(factor, event->pos());
    }
    else if(event->orientation() == Qt::Vertical)
    {
        this->_painterTranslate.setY(this->_painterTranslate.y()+event->delta());
        emit translated( - _painterTranslate.y());
        this->boundPainterTranslation();
        update();
    }
    else
    {
        this->_painterTranslate.setX(this->_painterTranslate.x()+event->delta());
        //emit translated( - _painterTranslate.y());
        this->boundPainterTranslation();
        update();
    }
}
void WidgetPdfDocument::zoom(qreal factor, QPoint target)
{
    this->_zoom *= factor;
    this->_painterTranslate *= factor;
    this->_painterTranslate += target - target*factor;
    this->boundPainterTranslation();
    this->refreshPages();

    this->_scroll->setRange(0,this->documentHeight() - this->height()+30);
    this->_scroll->setPageStep(this->height() / _zoom);
    emit translated( - _painterTranslate.y());
    initLinks();
    update();
}
void WidgetPdfDocument::zoomIn()
{
    this->zoom(1.1);
}

void WidgetPdfDocument::zoomOut()
{
    this->zoom(0.9);
}

void WidgetPdfDocument::mouseMoveEvent(QMouseEvent * event)
{
    this->checkLinksOver(event->posF());
    if(this->_mousePressed)
    {
        this->setCursor(Qt::ClosedHandCursor);
        this->_painterTranslate = this->_painterTranslateWhenMousePressed + (event->pos() - this->_pressAt);
        this->boundPainterTranslation();
        emit translated( - _painterTranslate.y());
        update();
    }

}
void WidgetPdfDocument::boundPainterTranslation()
{
    if(!this->_document || !this->_document->numPages())
    {
        return;
    }
    this->_painterTranslate.setX(max(this->_painterTranslate.x(), this->width() - this->_document->page(0)->pageSize().width()*_zoom - 30));
    this->_painterTranslate.setX(min(this->_painterTranslate.x(), 10));
    if(this->_document->page(0)->pageSize().width()*_zoom + 40 < this->width())
    {
        this->_painterTranslate.setX(-this->_document->page(0)->pageSize().width()*_zoom/2+this->width()/2-20);
    }

    this->_painterTranslate.setY(max(this->_painterTranslate.y(), this->height() - this->documentHeight() - 30));
    this->_painterTranslate.setY(min(this->_painterTranslate.y(), 10));
    if(this->documentHeight() < this->width())
    {
        this->_painterTranslate.setY(-this->documentHeight()/2+this->height()/2);
    }
}

void WidgetPdfDocument::updatePdf()
{
    this->initDocument();

    update();
}

void WidgetPdfDocument::jumpToEditorFromAbsolutePos(const QPointF &pos)
{
    if(!this->_document->numPages())
    {
        return;

    }
    QPointF absolute(pos - this->_painterTranslate);
    qreal pageHeightWithMargin = _document->page(0)->pageSize().height()*_zoom+WidgetPdfDocument::PageMargin;

    int page = absolute.y() / pageHeightWithMargin;
    QPointF relative(absolute.x(), absolute.y() - page * pageHeightWithMargin);

    if(relative.x() < 0 || relative.y() < 0)
    {
        return;
    }
    this->jumpToEditor(page, relative);
}

void WidgetPdfDocument::jumpToEditor(int page, const QPointF& pos)
{
    qDebug() << page << pos.x() << pos.y();
    if (scanner == NULL) return;
    if (synctex_edit_query(scanner, page+1, pos.x(), pos.y()) > 0)
    {
        synctex_node_t node;
        while ((node = synctex_next_result(scanner)) != NULL)
        {
            QString filename = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
            this->_widgetTextEdit->goToLine(synctex_node_line(node));
            break;
        }
    }
}

void WidgetPdfDocument::jumpToPdfFromSourceView(int /*top*/)
{
    if(!this->_widgetTextEdit->isCursorVisible() && _file)
    {
        int centerBlockNumber = this->_widgetTextEdit->centerBlockNumber();
        this->jumpToPdfFromSource(centerBlockNumber);
    }
}

void WidgetPdfDocument::jumpToPdfFromSource(int source_line)
{
    if (!_file) return;

    if(source_line == -1)
    {
        source_line = this->_widgetTextEdit->textCursor().blockNumber();
    }
    QString sourceFile = this->_file->getFilename();

    if(scanner == NULL)
    {
        return;
    }

    _widgetTextEdit->highlightSyncedLine(source_line);

    source_line = this->_file->getBuildedLine(source_line);

    if(source_line < 0 || source_line >= this->_widgetTextEdit->document()->blockCount())
    {
        return;
    }

    const QFileInfo sourceFileInfo(sourceFile);
    QDir curDir(QFileInfo(this->_file->getPdfFilename()).canonicalPath());
    synctex_node_t node = synctex_scanner_input(scanner);
    QString name;
    bool found = false;
    while (node != NULL)
    {
        name = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
        const QFileInfo fi(curDir, name);
        if (fi == sourceFileInfo)
        {
            found = true;
            break;
        }
        node = synctex_node_sibling(node);
    }
    if (!found)
    {
        return;
    }

    if (synctex_display_query(scanner, name.toUtf8().data(), source_line, 0) > 0)
    {
        int page = -1;
        path= QPainterPath();
        while ((node = synctex_next_result(scanner)) != NULL)
        {
            if (page == -1) page = synctex_node_page(node);
            if (synctex_node_page(node) != page) continue;
            QRectF nodeRect(synctex_node_box_visible_h(node),
                            synctex_node_box_visible_v(node) - synctex_node_box_visible_height(node),
                            synctex_node_box_visible_width(node),
                            synctex_node_box_visible_height(node) + synctex_node_box_visible_depth(node));
            path.addRect(nodeRect);
        }
        if (page > 0)
        {
            _syncPage = page - 1;
            _syncRect = path.boundingRect();
            goToPage(_syncPage, _syncRect.y(), _syncRect.height());
            _lastUpdate.start();
            _timer.start(1);
        }
    }
}
