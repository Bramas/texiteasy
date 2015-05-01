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

#ifndef WIDGETPDFDOCUMENT_H
#define WIDGETPDFDOCUMENT_H

#include <QWidget>
#include <QPoint>
#include <QElapsedTimer>
#include <QTimer>
#include <QList>
#include <QRectF>
#include <QScrollBar>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#if QT_VERSION > QT_VERSION_CHECK(5,2,0)
#    include <QNativeGestureEvent>
#endif

#include "synctex_parser.h"
#include <QPoint>

#ifdef OS_MAC
#include "poppler/qt5/poppler-qt5.h"
#else
    #ifdef OS_WINDOWS
        #include <poppler/qt5/poppler-qt5.h>
    #else
        #include "poppler/qt4/poppler-qt4.h"
    #endif
#endif

class File;
class QImage;
class WidgetTextEdit;

struct Link
{
    QRectF rectangle;
    Poppler::LinkGoto * destination;

    ~Link()
    {
        //delete destination;
    }
};



class WidgetPdfDocument : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetPdfDocument(QWidget *parent = 0);
    ~WidgetPdfDocument();
    void setFile(File * file) { this->_file = file; this->initDocument(); }
    void setWidgetTextEdit(WidgetTextEdit * widgetTextEdit) { this->_widgetTextEdit = widgetTextEdit; }


    /**
     * @brief goToPage
     * @param page page of the area we want to view
     * @param top of the area we want to view (relative to the page)
     * @param height of the area we want to view
     */
    void goToPage(int page, int top=0, int height=0);

    /**
     * @brief documentHeight
     * @return the document height. the sum of all pages' height taking zoom into account and with margins between pages
     */
    int documentHeight()
    {
        if(!_document) return 0;
        return (_documentHeight + WidgetPdfDocument::PageMargin * (_document->numPages() - 1))*_zoom ;
    }
    void updateScrollBar();
    qreal zoom() { return _zoom; }
    QPoint pdfOffset() { return this->_painterTranslate; }
    void setZoom(qreal zoom) { _zoom = zoom; }
    void setPdfOffset(QPoint p) { this->_painterTranslate = p; }
signals:
    void translated(int);
public slots:
    void jumpToPdfFromSourceView(int);
    void jumpToPdfFromSource(int source_line = -1);
    void zoomIn();
    void zoomOut();
    void zoom(qreal factor, QPoint target = QPoint(0,0));
    void updatePdf(void);
    void initScroll();
    void onScroll(int value);
private slots:
    void onSyncReady(int page, QRectF rect);
protected:
#if QT_VERSION > QT_VERSION_CHECK(5,2,0)
    bool gestureEvent(QNativeGestureEvent* event);
#endif
    bool event(QEvent * event);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);
    void jumpToEditor(int page, const QPoint& pos);
    void jumpToEditorFromAbsolutePos(const QPoint &pos);

private slots:
    void refreshPages();
private:

    void initDocument();
    void initLinks();
    void boundPainterTranslation();
    QImage * page(int page);
    void checkLinksOver(const QPointF &pos);
    bool checkLinksPress(const QPointF &pos);


    Poppler::Document* _document;
    int _documentHeight;
    static QImage * EmptyImage;
    File* _file;
    QElapsedTimer _lastUpdate;
    QList<Link> _links;
    bool * _loadedPages;
    bool _mousePressed;
    static int PageMargin;
    QImage ** _pages;
    QPainterPath path;
    QPoint _mousePosition;
    QPoint _pressAt;
    QPoint _painterTranslate;
    QPoint _painterTranslateWhenMousePressed;
    synctex_scanner_t scanner;
    QScrollBar * _scroll;
    int _syncPage;
    QRectF _syncRect;
    QTimer _timer;
    QTimer _requestNewResolutionTimer;
    WidgetTextEdit * _widgetTextEdit;
    qreal _zoom;



};

#endif // WIDGETPDFDOCUMENT_H
