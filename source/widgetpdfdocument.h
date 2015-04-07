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
        #include "poppler-qt5/poppler-qt5.h"
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
    Poppler::LinkDestination * destination;

    ~Link()
    {
        //delete destination;
    }
};




class PdfSynchronizer : public QThread
{
    Q_OBJECT

public:

    PdfSynchronizer() : _restartRequested(false), _syncing(false), _waitBeforeSync(false) {}

    bool lockBeforeSync() {

        qDebug()<<"lockBeforeSync _dataMutex before";
        _dataMutex.lock();
        qDebug()<<"lockBeforeSync _dataMutex locked";
        _waitBeforeSync = true;
        if(!_syncing)
        {
            qDebug()<<"lockBeforeSync _dataMutex unlocked";
            _dataMutex.unlock();
            return true;
        }
        _runningWaiter.wait(&_dataMutex);
        qDebug()<<"lockBeforeSync _dataMutex locked";
        qDebug()<<"lockBeforeSync _dataMutex unlocked";
        _dataMutex.unlock();
        return true;

    }
    void unlockBeforeSync() {

        qDebug()<<"unlockBeforeSync _dataMutex before";
        _dataMutex.lock();
        qDebug()<<"unlockBeforeSync _dataMutex locked";
        _waitBeforeSync = false;
        _runningWaiter.wakeAll();
        qDebug()<<"unlockBeforeSync _dataMutex unlocked";
        _dataMutex.unlock();

    }

    void sync(synctex_scanner_t scanner, QString sourceFile, int sourceLine){

        qDebug()<<"sync _dataMutex before";
        _dataMutex.lock();
        qDebug()<<"sync _dataMutex locked";

        _scanner = scanner;
        _sourceLine = sourceLine;
        _sourceFile = sourceFile;
        _restartRequested = true;

        _dataWaiter.wakeAll();
        _dataMutex.unlock();
        qDebug()<<"sync _dataMutex unlocked";
    }

    void waitForFinish()
    {
        qDebug()<<"waitForFinish _dataMutex before";
        _dataMutex.lock();
        qDebug()<<"waitForFinish _dataMutex locked";
        _stopRequested = true;
        qDebug()<<"Stop! syncing?"<<_syncing;
        bool s = _syncing;
        qDebug()<<"waitForFinish _dataMutex unlocked ";
        _dataMutex.lock();
        if(!s)
        {
            _dataWaiter.wakeAll();
        }
        //_dataMutex.lock();
        qDebug()<<"waitForFinish _dataMutex locked";
        qDebug()<<"waitForFinish _dataMutex unlocked ";
        _finishWaiter.wait(&_dataMutex);
        qDebug()<<"waitForFinish _dataMutex locked";
        _dataMutex.unlock();
        qDebug()<<"waitForFinish _dataMutex unlocked";
        qDebug()<<"Stop waiting";
    }

private:


    void run() Q_DECL_OVERRIDE {

        begin:
        _runningWaiter.wakeAll();
        qDebug()<<"run _dataMutex before";
        _dataMutex.lock();
        qDebug()<<"run _dataMutex locked";
        if(_waitBeforeSync)
        {
            qDebug()<<"run _dataMutex unlocked";
            _runningWaiter.wait(&_dataMutex);
            qDebug()<<"run _dataMutex locked";
        }

        _syncing = false;
        if(!_restartRequested)
        {
            qDebug()<<"run _dataMutex unlocked";
            _dataWaiter.wait(&_dataMutex);
            qDebug()<<"run _dataMutex locked";
            qDebug()<<"run _dataMutex unlocked";
            //_dataMutex.unlock();
            qDebug()<<"run _stopRequested "<<_stopRequested;
            if(_stopRequested)
            {
                _finishWaiter.wakeAll();
                _dataMutex.unlock();
                qDebug()<<"run _finishWaiter.wakeAll(); ";
                return;
            }
            //_dataMutex.lock();
            if(_waitBeforeSync)
            {
                qDebug()<<"run _dataMutex unlocked";
                _runningWaiter.wait(&_dataMutex);
                qDebug()<<"_dataMutex locked";
            }
        }
        _syncing = true;
        int sourceLine = _sourceLine;
        QString sourceFile = _sourceFile;
        synctex_scanner_t scanner = _scanner;
        _restartRequested = false;
        _sourceLine = -1;

        qDebug()<<"run _dataMutex unlocked";
        _dataMutex.unlock();
        qDebug()<<"run START SYNC";

        if(scanner == NULL)
        {
            goto begin;
        }

        const QFileInfo sourceFileInfo(sourceFile);
        QDir curDir(sourceFileInfo.canonicalPath());
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


        if (found && synctex_display_query(scanner, name.toUtf8().data(), sourceLine, 0) > 0)
        {
            int page = -1;
            QPainterPath path;
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
                emit rectSync(page - 1, path.boundingRect());
            }
        }
        goto begin;
    }
signals:
    void rectSync(int page, QRectF rect);
private:
    QMutex _dataMutex;
    QWaitCondition _dataWaiter;
    QWaitCondition _runningWaiter;
    QWaitCondition _finishWaiter;
    bool _restartRequested, _syncing, _waitBeforeSync, _stopRequested;
    QString _sourceFile;
    int _sourceLine;
    synctex_scanner_t _scanner;
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
    PdfSynchronizer * _pdfSynchronizer;



};

#endif // WIDGETPDFDOCUMENT_H
