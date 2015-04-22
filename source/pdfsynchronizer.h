#ifndef PDFSYNCHRONIZER_H
#define PDFSYNCHRONIZER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRectF>
#include <QPoint>
#include <QPainterPath>

#include "synctex_parser.h"

#define PDF_SYNCHRONIZER_DEBUG(a)

class PdfSynchronizer : public QThread
{
    Q_OBJECT

    PdfSynchronizer() : _restartRequested(false),
        _syncing(false),
        _waitBeforeSync(false),
        _stopRequested(false)
    {}

    static PdfSynchronizer Instance;

public:

    static bool lockBeforeSync() {
        return Instance._lockBeforeSync();
    }
    static void unlockBeforeSync() {
        Instance._unlockBeforeSync();
    }
    static void sync(QObject * receiver, QString methodName, synctex_scanner_t scanner, QString sourceFile, int sourceLine){
        Instance._sync(receiver, methodName, scanner, sourceFile, sourceLine);
    }
    static void terminate() {
        Instance._terminate();
    }

    static void start() {
        Instance._start();
    }

    static bool wait() {
        return Instance._wait();
    }

private:

    void _start()
    {
        QThread::start();
    }
    bool _wait()
    {
        return QThread::wait();
    }

    void _terminate()
    {
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"waitForFinish _dataMutex before");
        _dataMutex.lock();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"waitForFinish _dataMutex locked");
        _stopRequested = true;
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"Stop! syncing?"<<_syncing);
        bool s = _syncing;

        _dataMutex.unlock();
        if(!s)
        {
            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"not syncing: waitForFinish _dataMutex unlocked and _dataWaiter.wakeAll();");
            _dataWaiter.wakeAll();
            return;
        }
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"waitForFinish _syncing so _dataMutex unlocked ");
        _dataMutex.unlock();
    }



    bool _lockBeforeSync() {

        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex before");
        _dataMutex.lock();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex locked");
        _waitBeforeSync = true;
        if(!_syncing)
        {
            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex unlocked");
            _dataMutex.unlock();
            return true;
        }
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex unlocked");
        _runningWaiter.wait(&_dataMutex);
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex locked");
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"lockBeforeSync _dataMutex unlocked");
        _dataMutex.unlock();
        return true;

    }
    void _unlockBeforeSync() {

        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"unlockBeforeSync _dataMutex before");
        _dataMutex.lock();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"unlockBeforeSync _dataMutex locked");
        _waitBeforeSync = false;
        _runningWaiter.wakeAll();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"unlockBeforeSync _dataMutex unlocked");
        _dataMutex.unlock();

    }

    void _sync(QObject * receiver, QString methodName, synctex_scanner_t scanner, QString sourceFile, int sourceLine){

        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"sync _dataMutex before");
        _dataMutex.lock();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"sync _dataMutex locked");

        _scanner = scanner;
        _sourceLine = sourceLine;
        _sourceFile = sourceFile;
        _restartRequested = true;


        _receiver = receiver;
        _methodName = methodName;

        _dataWaiter.wakeAll();
        PDF_SYNCHRONIZER_DEBUG(qDebug()<<"sync _dataMutex unlocked");
        _dataMutex.unlock();
    }


    void run() Q_DECL_OVERRIDE {

       forever
       {
            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex before");
            _dataMutex.lock();
            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex locked");
            if(_stopRequested)
            {
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _stopRequested");
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex.unlock(); ");
                _dataMutex.unlock();
                return;
            }
            if(_waitBeforeSync)
            {
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"_runningWaiter.wakeAll();");
                _runningWaiter.wakeAll();
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex unlocked");
                _runningWaiter.wait(&_dataMutex);
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex locked");
            }

            _syncing = false;
            if(!_restartRequested)
            {
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex unlocked");
                _dataWaiter.wait(&_dataMutex);
                PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex locked");
                if(_stopRequested)
                {
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _stopRequested");
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex.unlock(); ");
                    _dataMutex.unlock();
                    return;
                }
                if(_waitBeforeSync)
                {
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _waitBeforeSync");
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex unlocked");
                    _runningWaiter.wait(&_dataMutex);
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"_dataMutex locked");
                }
            }
            _syncing = true;
            int sourceLine = _sourceLine;
            QString sourceFile = _sourceFile;
            synctex_scanner_t scanner = _scanner;

            const QObject * receiver = _receiver;
            QString methodName = _methodName.toLatin1().data();

            _restartRequested = false;
            _sourceLine = -1;

            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run _dataMutex unlocked");
            _dataMutex.unlock();
            PDF_SYNCHRONIZER_DEBUG(qDebug()<<"run START SYNC");

            if(scanner == NULL)
            {
                continue;
            }
#ifdef OS_MAC
            QString filePath=QFileInfo(sourceFile).absolutePath()+"/./"+QFileInfo(sourceFile).fileName();
#else
            QString filePath=QFileInfo(sourceFile).canonicalFilePath().replace("/", "\\");
#endif
            synctex_node_t node = synctex_scanner_input(scanner);
            QString name;
            bool found = false;
            while (node != NULL)
            {
                name = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
                if (name == filePath)
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
                    QMetaObject::invokeMethod(const_cast<QObject *>(receiver),
                                              methodName.toLatin1().constData(), Qt::QueuedConnection,
                                              Q_ARG( int, page - 1),
                                              Q_ARG( QRectF, path.boundingRect())
                                              );
                    PDF_SYNCHRONIZER_DEBUG(qDebug()<<"QMetaObject::invokeMethod    "<<methodName);
                    //emit rectSync(page - 1, path.boundingRect());
                }
            }
        }
    }
private:
    QMutex _dataMutex;
    QWaitCondition _dataWaiter;
    QWaitCondition _runningWaiter;
    QWaitCondition _finishWaiter;
    bool _restartRequested, _syncing, _waitBeforeSync, _stopRequested;
    QString _sourceFile;
    int _sourceLine;
    synctex_scanner_t _scanner;
    QObject *_receiver;
    QString _methodName;
};


#endif // PDFSYNCHRONIZER_H
